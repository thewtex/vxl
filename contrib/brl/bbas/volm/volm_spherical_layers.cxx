#include "volm_spherical_layers.h"
#include "volm_io.h"
#include <bpgl/depth_map/depth_map_scene.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_shell_container.h>
#include <bsol/bsol_algs.h>

#define TOL -1E-8
volm_spherical_layers::
volm_spherical_layers(vpgl_perspective_camera<double> const& cam,
			depth_map_scene_sptr const& dm_scene,
		      double altitude,
		      volm_spherical_container_sptr const& sph_vol,
		      volm_spherical_shell_container_sptr const& sph_shell,
		      unsigned char invalid, 
		      unsigned char default_sky_order,
		      double d_threshold,
		      unsigned log_downsample_ratio
		      ): cam_(cam), dm_scene_(dm_scene),altitude_(altitude),
  sph_vol_(sph_vol), sph_shell_(sph_shell), invalid_(invalid),
  default_sky_order_(default_sky_order), d_threshold_(d_threshold),
  log_downsample_ratio_(log_downsample_ratio){
}
							
unsigned char volm_spherical_layers::
fetch_depth(double const& u, double const& v,
	    vcl_vector<depth_map_region_sptr> const& depth_regions,
	    vcl_vector<depth_map_region_sptr> const& ground_plane,
	    vcl_vector<depth_map_region_sptr> const& sky,
	    unsigned char& order, unsigned char& max_dist,
	    unsigned& object_id,  unsigned char& grd_nlcd,
	    bool& is_ground,  bool& is_sky,
	    bool& is_object,  vil_image_view<float> const& gp_depth_img)
{
  unsigned char min_dist;
  // check other objects before ground,
  // e.g.,  for overlap region of a building and ground, building is on the ground and it is must closer than the ground
  // find if (u, v) is contained in non-ground, non-sky region
  double min_res = sph_vol_->min_voxel_res();
  unsigned depth_reg_size = (unsigned)depth_regions.size();
  if (depth_reg_size) {
    for (unsigned i = 0; i < depth_reg_size; ++i) {
      vgl_polygon<double> poly = 
	bsol_algs::vgl_from_poly(depth_regions[i]->region_2d());
      if (poly.contains(u,v)) {
        is_object = true;
        object_id = i;
        double min_depth = depth_regions[i]->min_depth();
        if (min_depth < min_res)
          min_dist = invalid_;
        else
          min_dist = sph_vol_->get_depth_interval(min_depth);
        double max_depth = depth_regions[i]->max_depth();
        if (max_depth < min_res)
          max_dist = invalid_;
        else
          max_dist = sph_vol_->get_depth_interval(max_depth);
        order = (unsigned char)depth_regions[i]->order();
        return min_dist;
      }
    }
  }
  // no object regions, check if ground plane exists
  unsigned gp_size = ground_plane.size();
  if (gp_size) {
    for (unsigned i = 0; i < gp_size; ++i) {
      vgl_polygon<double> vgl_ground = 
	bsol_algs::vgl_from_poly(ground_plane[i]->region_2d());
      if (vgl_ground.contains(u,v)) {
        is_ground = true;
        // get the depth of the pixel
        // maybe better to do bilinear interpolation instead of casting to nearest pixel
        int uu = (int)vcl_floor(u/(1<<log_downsample_ratio_)+0.5);
        uu = uu < 0 ? 0 : uu;
        uu = uu >= (int)dm_scene_->ni() ? dm_scene_->ni()-1 : uu;
        int vv = (int)vcl_floor(v/(1<<log_downsample_ratio_)+0.5);
        vv = vv < 0 ? 0 : vv;
        vv = vv >= (int)dm_scene_->nj() ? dm_scene_->nj()-1 : vv;
        float depth_uv = gp_depth_img(uu,vv);
        // handle the case where the voxel/ray is too close to ground_plane boundary
        if (depth_uv < 0) {
#ifdef DEBUG
          vcl_cout << " WARNING: point (" << (int)u << ',' << (int)v << ") "
                   << " is too close to the ground boundary, disregard" << vcl_endl;
#endif
          is_ground = false;
          max_dist = invalid_;
          order = invalid_;
	    return (unsigned char)253; // invalid depth value
        }
        min_dist = sph_vol_->get_depth_interval(depth_uv);
        max_dist = invalid_;
        order = (unsigned char)(ground_plane[i]->order());
        grd_nlcd = (unsigned char)volm_nlcd_table::land_id[ground_plane[i]->nlcd_id()];
        return min_dist;
      }
    }
  }
  // check if (u, v) is contained in sky 
  // considered last since all objects should be closer than sky
  unsigned sky_size = sky.size();
  if (sky_size) {
    for (unsigned i = 0; i < sky_size; ++i) {
      vgl_polygon<double> vgl_sky = 
	bsol_algs::vgl_from_poly(sky[i]->region_2d());
      if (vgl_sky.contains(u,v)) {
        is_sky = true;
        max_dist = (unsigned char)254;
        order = default_sky_order_;
        return (unsigned char)254;
      }
    }
  }
  // the image point (u,v) corresponding to the current ray
  // is not inside any defined object region
  max_dist = invalid_;
  order = invalid_;
  return invalid_;
}

bool volm_spherical_layers::compute_layers(){
  vcl_cout << "layers camera \n" << cam_ << '\n';
  vcl_vector<depth_map_region_sptr> scn_regs = dm_scene_->scene_regions();  
  dist_id_layer_.resize(scn_regs.size());
  vcl_vector<depth_map_region_sptr> gp_regs = dm_scene_->ground_plane();  
  vcl_vector<depth_map_region_sptr> sky_regs = dm_scene_->sky();  
  vcl_vector<vgl_point_3d<double> > rays = sph_shell_->cart_points();
  unsigned n_rays = (unsigned)rays.size();
  vil_image_view<float> gp_depth_img = dm_scene_->depth_map("ground_plane", log_downsample_ratio_, d_threshold_);
  unsigned count = 0;
  for (unsigned ray_idx = 0; ray_idx < n_rays; ++ray_idx) {
    vgl_point_3d<double> ray(rays[ray_idx].x(), rays[ray_idx].y(), rays[ray_idx].z()+altitude_);
    unsigned char min_dist, order, max_dist;
    // check whether the point is behind the camera
    if (cam_.is_behind_camera(vgl_homg_point_3d<double>(ray))) {
      min_dist_layer_.push_back(invalid_);
      max_dist_layer_.push_back(invalid_);
      order_layer_.push_back(invalid_);
    }
    else {
      double u, v;
      cam_.project(ray.x(), ray.y(), ray.z(), u, v);
      if ( u > (double)dm_scene_->ni() ||
	   v > (double)dm_scene_->nj() || 
	   u < TOL || v < TOL) {   // container point ray is outside camera viewing volume
	min_dist_layer_.push_back(invalid_);
	max_dist_layer_.push_back(invalid_);
	order_layer_.push_back(invalid_);
      }else{
	bool is_ground = false, is_sky = false, is_object = false;
	unsigned obj_id;
	unsigned char grd_nlcd;
	min_dist = this->fetch_depth(u, v, 
				     scn_regs, gp_regs, sky_regs,
				     order, max_dist,
				     obj_id, grd_nlcd, is_ground,
				     is_sky, is_object, gp_depth_img);
	vcl_cout << ray_idx << ' ' << count << ' '
		 << (unsigned)min_dist << ' ' << u << ' ' << v << ' ' 
		 << (unsigned)order << ' ' << (unsigned)max_dist 
		 << ' ' << (unsigned)obj_id << ' ' << (unsigned)grd_nlcd 
		 << ' ' << is_ground << ' ' << is_sky << ' ' 
		 << is_object << '\n';
	min_dist_layer_.push_back(min_dist);
	max_dist_layer_.push_back(max_dist);
	order_layer_.push_back(order);
	if (is_ground) {
	  ground_id_layer_.push_back(ray_idx);
	  ground_dist_layer_.push_back(min_dist);
	  ground_nlcd_layer_.push_back(grd_nlcd);
	}
	else if (is_sky) {
	  sky_id_layer_.push_back(ray_idx);
	}
	else if (is_object){
	  if (obj_id < scn_regs.size()) {
	    dist_id_layer_[obj_id].push_back(ray_idx);
	  }
	  else {
	    vcl_cerr << "ERROR in spherical layer creation: " <<
	      "object id exceeds the size of non-ground, non-sky objects\n";
	    return false;
	  }
	}
	if ((unsigned)min_dist != (unsigned)invalid_)
	  ++count;
      }
    }
  } // loop over rays for current camera
  return true;
}

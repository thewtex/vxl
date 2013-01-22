//This is brl/bbas/volm/volm_spherical_layers.h
#ifndef volm_spherical_layers_h_
#define volm_spherical_layers_h_
//:
// \file
// \brief A class to store an axis-aligned spherical query region
// Units are in meters
//
// \author J.L. Mundy
// \date January 21, 2012
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_box_2d.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_region_sptr.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
class volm_spherical_layers
{
 public:
  volm_spherical_layers(vpgl_perspective_camera<double> const& cam,
			depth_map_scene_sptr const& dm_scene,
			double altitude,
			volm_spherical_container_sptr const& sph_vol,
			volm_spherical_shell_container_sptr const& sph_shell,
			unsigned char invalid,
			unsigned char default_sky_order,
			double d_threshold,
			unsigned log_downsample_ratio);
		   
  //: accessors
  //: the minimum distance for each ray
  const vcl_vector<unsigned char>& min_dist_layer() const
    {return min_dist_layer_;}
  //: the maximum distance for each ray
  const vcl_vector<unsigned char>& max_dist_layer() const
    {return max_dist_layer_;}
  //: the region order for each ray
  const vcl_vector<unsigned char>& order_layer() const
    {return order_layer_;}
  //: the set of rays intersecting the ground plane
  const vcl_vector<unsigned>& ground_id_layer() const
    {return ground_id_layer_;}
  //: the ground plane distance for each ray
  const vcl_vector<unsigned char>& ground_dist_layer() const
    {return ground_dist_layer_;}
  //: the ground plane land class for each gp ray
  const vcl_vector<unsigned char>& ground_nlcd_layer() const
    {return ground_nlcd_layer_;}
  //: the set of rays intersecting sky
  const vcl_vector<unsigned>& sky_id_layer() const
    {return sky_id_layer_;}
  //: the set of rays for each non-gp, non-sky region
  const vcl_vector<vcl_vector<unsigned> >& dist_id_layer() const
    {return dist_id_layer_;}
  //: the current camera
  vpgl_perspective_camera<double> camera() const
    {return cam_;}
  //: the count of valid rays
  unsigned count() const {return count_;}
  //: compute shell layers
  bool compute_layers();
 private:
  unsigned char fetch_depth(double const& u, double const& v,
			    vcl_vector<depth_map_region_sptr> const& depth_regions,
			    vcl_vector<depth_map_region_sptr> const& ground_plane,
			    vcl_vector<depth_map_region_sptr> const& sky,
			    unsigned char& order, unsigned char& max_dist,
			    unsigned& object_id,  unsigned char& grd_nlcd,
			    bool& is_ground,  bool& is_sky,
			    bool& is_object,  
			    vil_image_view<float> const& gp_depth_img);


  vpgl_perspective_camera<double> cam_;  
  depth_map_scene_sptr dm_scene_;
  double altitude_;
  volm_spherical_container_sptr sph_vol_;
  volm_spherical_shell_container_sptr sph_shell_;
  unsigned char invalid_;
  unsigned char default_sky_order_;
  unsigned log_downsample_ratio_;
  double d_threshold_;
  vcl_vector<unsigned char> min_dist_layer_;
  vcl_vector<unsigned char> max_dist_layer_;
  vcl_vector<unsigned char> order_layer_;
  vcl_vector<unsigned> ground_id_layer_;
  vcl_vector<unsigned char> ground_dist_layer_;
  vcl_vector<unsigned char> ground_nlcd_layer_;
  vcl_vector<unsigned> sky_id_layer_;
  vcl_vector<vcl_vector<unsigned> > dist_id_layer_;
  unsigned count_;
};
#endif //spherical_layers_h_

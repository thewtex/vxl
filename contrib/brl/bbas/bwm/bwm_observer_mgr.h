#ifndef bwm_observer_mgr_h_
#define bwm_observer_mgr_h_

#include <vcl_vector.h>

#include "bwm_observer.h"
#include "bwm_observer_cam.h"
#include "bwm_observable.h"
#include "bwm_observer_rat_cam.h"
#include "bwm_corr.h"
#include "bwm_corr_sptr.h"


class bwm_observer_mgr
{
public:
  typedef enum {IMAGE_TO_IMAGE, WORLD_TO_IMAGE} BWM_CORR_MODE;

  static bwm_observer_mgr* instance();

  virtual ~bwm_observer_mgr() {}

  static bwm_observer_cam* BWM_MASTER_OBSERVER;

  vcl_vector<bwm_observer_rat_cam*> observers_rat_cam();

  void add(bwm_observer* o){ observers_.push_back(o); }

  void remove(bwm_observer* observer);

  //: attach the observable to all observers
  void attach(bwm_observable_sptr obs);

  // Correspondence methods
  void collect_corr();
  void set_corr(bwm_corr_sptr corr);
  void set_world_pt(vgl_point_3d<double> world_pt);
  bool obs_in_corr(bwm_observer_cam *obs);
  void save_corr(vcl_ostream& s);
  void save_corr_XML();
  void update_corr(bwm_observer_cam* obs,vgl_point_2d<double> old_pt,vgl_point_2d<double> new_pt);
  void delete_last_corr();
  void delete_all_corr();
  BWM_CORR_MODE corr_mode() { return corr_mode_; }
  void set_corr_mode();
  void set_corr_mode(BWM_CORR_MODE mode){corr_mode_ = mode;}
  void move_to_corr();

  //: picking up corr points are controlled by starting and stopping it
  void start_corr() { start_corr_ = true; }
  void stop_corr() { start_corr_ = false; }
  //: returns true if the correspondence picking started by the main corr menu
  bool in_corr_picking() { return start_corr_; }

  void print_observers();

  vcl_vector<bwm_corr_sptr> correspondences()
    {return corr_list_;}
  
  //: Given a set of image-to-image correpondences
  //  solve for the 3-d world point and adjust the cameras
  void adjust_camera_offsets();

private:
  bwm_observer_mgr() : start_corr_(true), world_point_valid_(false) 
    {corr_mode_ = IMAGE_TO_IMAGE;}

  static bwm_observer_mgr* instance_;
  
  vcl_vector<bwm_observer* > observers_;

  bool start_corr_;
  BWM_CORR_MODE corr_mode_;
  bool world_point_valid_;
  vcl_vector<bwm_corr_sptr> corr_list_;
  vgl_point_3d<double> corr_world_pt_;
};

#endif

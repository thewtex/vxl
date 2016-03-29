#include <bsta/algo/bsta_adaptive_updater.hxx>
#include <bsta/bsta_gauss_sf1.h>

typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3> mix_fix_gauss_sf1_3;

BSTA_ADAPTIVE_UPDATER_INSTANTIATE(mix_fix_gauss_sf1_3);

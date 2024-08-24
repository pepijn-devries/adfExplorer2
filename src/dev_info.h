#ifndef __DEV_INFO_ADF__
#define __DEV_INFO_ADF__

#include "connect_adf.h"

void check_volume_number(AdfDevice *dev, int vol_num);
cpp11::r_string adf_dev_name_internal(AdfDevice * dev, int vol_num);
[[cpp11::register]] cpp11::r_string adf_dev_name(SEXP connection, int vol_num);
[[cpp11::register]] int adf_dev_size(SEXP connection);
[[cpp11::register]] int adf_dev_nvol(SEXP connection);
[[cpp11::register]] cpp11::r_string adf_dev_type(SEXP connection);
[[cpp11::register]] int adf_free_blocks(SEXP connection, int vol_num);
[[cpp11::register]] int adf_block_size(SEXP connection, int vol_num);
[[cpp11::register]] int adf_vol_size(SEXP connection, int vol_num);
[[cpp11::register]] cpp11::r_bool adf_is_write_protected(SEXP connection);
[[cpp11::register]] cpp11::r_bool adf_is_bootable(SEXP connection, int vol_num);
[[cpp11::register]] cpp11::r_bool adf_is_intl(SEXP connection, int vol_num);
[[cpp11::register]] cpp11::r_bool adf_is_dircache(SEXP connection, int vol_num);
[[cpp11::register]] cpp11::r_bool adf_is_ffs(SEXP connection, int vol_num);

#endif
#ifndef __FILE_INFO_ADF__
#define __FILE_INFO_ADF__
#pragma once

/* masks for different modes for getting entry from path name */
#define ADF_FI_EXPECT_FILE                0b1
#define ADF_FI_EXPECT_DIR                0b10
#define ADF_FI_THROW_ERROR              0b100
#define ADF_FI_WARN                    0b1000
#define ADF_FI_EXPECT_EXIST           0b10000
#define ADF_FI_EXPECT_VALID_CHECKSUM 0b100000

#include <sstream>
#include "dev_info.h"
using namespace cpp11;

[[cpp11::register]] r_string adf_upper(strings x, logicals intl);
[[cpp11::register]] list adf_path_to_entry(SEXP connection, strings filename, int mode);
[[cpp11::register]] r_string adf_entry_to_path(SEXP connection, int vol_num, int sectnum, bool full);
[[cpp11::register]] list list_adf_entries_(SEXP connection, strings filename, logicals recursive);
[[cpp11::register]] SEXP adf_change_dir(SEXP connection, strings path);
[[cpp11::register]] SEXP adf_get_current_dir(SEXP connection);
[[cpp11::register]] SEXP adf_mkdir(SEXP connection, r_string path);
[[cpp11::register]] SEXP adf_remove_entry(SEXP connection, strings path, logicals flush);
[[cpp11::register]] SEXP move_adf_internal(SEXP connection, strings source, strings destination);
[[cpp11::register]] SEXP adf_dir_exists_(SEXP connection, strings path);
[[cpp11::register]] SEXP adf_file_exists_(SEXP connection, strings path);
[[cpp11::register]] SEXP adf_entry_info_(SEXP connection, strings path);
r_string adf_entry_to_path_internal(AdfDevice * dev, int vol_num, int sectnum, bool full);
list list_adf_entries2_(SEXP connection, AdfVolume * vol,
                    SECTNUM sector, int vol_num, r_bool recursive);
bool adf_check_volume(AdfDevice * dev, std::string vol_name, int & cur_vol, int & cur_pos);
void adf_change_dir_internal(SEXP connection, SECTNUM sector, int volume);

#endif /* __FILE_INFO_ADF__ */

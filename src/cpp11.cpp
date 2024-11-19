// Generated by cpp11: do not edit by hand
// clang-format off


#include "cpp11/declarations.hpp"
#include <R_ext/Visibility.h>

// adf_file_info.cpp
std::string adf_upper(std::string x, bool intl);
extern "C" SEXP _adfExplorer2_adf_upper(SEXP x, SEXP intl) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_upper(cpp11::as_cpp<cpp11::decay_t<std::string>>(x), cpp11::as_cpp<cpp11::decay_t<bool>>(intl)));
  END_CPP11
}
// adf_file_info.cpp
list adf_path_to_entry(SEXP connection, std::string filename, int mode);
extern "C" SEXP _adfExplorer2_adf_path_to_entry(SEXP connection, SEXP filename, SEXP mode) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_path_to_entry(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(filename), cpp11::as_cpp<cpp11::decay_t<int>>(mode)));
  END_CPP11
}
// adf_file_info.cpp
bool adf_file_exists_(SEXP connection, std::string path);
extern "C" SEXP _adfExplorer2_adf_file_exists_(SEXP connection, SEXP path) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_file_exists_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path)));
  END_CPP11
}
// adf_file_info.cpp
bool adf_dir_exists_(SEXP connection, std::string path);
extern "C" SEXP _adfExplorer2_adf_dir_exists_(SEXP connection, SEXP path) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_dir_exists_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path)));
  END_CPP11
}
// adf_file_info.cpp
list adf_entry_info_(SEXP connection, std::string path);
extern "C" SEXP _adfExplorer2_adf_entry_info_(SEXP connection, SEXP path) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_entry_info_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path)));
  END_CPP11
}
// adf_file_operators.cpp
SEXP adf_change_dir(SEXP connection, std::string path);
extern "C" SEXP _adfExplorer2_adf_change_dir(SEXP connection, SEXP path) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_change_dir(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path)));
  END_CPP11
}
// adf_file_operators.cpp
SEXP adf_get_current_dir(SEXP connection);
extern "C" SEXP _adfExplorer2_adf_get_current_dir(SEXP connection) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_get_current_dir(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection)));
  END_CPP11
}
// adf_file_operators.cpp
list list_adf_entries_(SEXP connection, std::string filename, bool recursive, bool nested);
extern "C" SEXP _adfExplorer2_list_adf_entries_(SEXP connection, SEXP filename, SEXP recursive, SEXP nested) {
  BEGIN_CPP11
    return cpp11::as_sexp(list_adf_entries_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(filename), cpp11::as_cpp<cpp11::decay_t<bool>>(recursive), cpp11::as_cpp<cpp11::decay_t<bool>>(nested)));
  END_CPP11
}
// adf_file_operators.cpp
SEXP adf_mkdir(SEXP connection, std::string path);
extern "C" SEXP _adfExplorer2_adf_mkdir(SEXP connection, SEXP path) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_mkdir(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path)));
  END_CPP11
}
// adf_file_operators.cpp
SEXP adf_remove_entry(SEXP connection, std::string path, bool flush);
extern "C" SEXP _adfExplorer2_adf_remove_entry(SEXP connection, SEXP path, SEXP flush) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_remove_entry(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path), cpp11::as_cpp<cpp11::decay_t<bool>>(flush)));
  END_CPP11
}
// adf_file_operators.cpp
SEXP adf_set_entry_name_(SEXP connection, std::string path, std::string replacement);
extern "C" SEXP _adfExplorer2_adf_set_entry_name_(SEXP connection, SEXP path, SEXP replacement) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_set_entry_name_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(path), cpp11::as_cpp<cpp11::decay_t<std::string>>(replacement)));
  END_CPP11
}
// adf_file_operators.cpp
SEXP move_adf_internal(SEXP connection, std::string source, std::string destination);
extern "C" SEXP _adfExplorer2_move_adf_internal(SEXP connection, SEXP source, SEXP destination) {
  BEGIN_CPP11
    return cpp11::as_sexp(move_adf_internal(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(source), cpp11::as_cpp<cpp11::decay_t<std::string>>(destination)));
  END_CPP11
}
// block_adf.cpp
SEXP read_adf_block_(SEXP connection, int sector);
extern "C" SEXP _adfExplorer2_read_adf_block_(SEXP connection, SEXP sector) {
  BEGIN_CPP11
    return cpp11::as_sexp(read_adf_block_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(sector)));
  END_CPP11
}
// block_adf.cpp
SEXP write_adf_block_(SEXP connection, int sector, raws block);
extern "C" SEXP _adfExplorer2_write_adf_block_(SEXP connection, SEXP sector, SEXP block) {
  BEGIN_CPP11
    return cpp11::as_sexp(write_adf_block_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(sector), cpp11::as_cpp<cpp11::decay_t<raws>>(block)));
  END_CPP11
}
// block_adf.cpp
list interpret_file_header(SEXP connection, int vol_num, int sectnum);
extern "C" SEXP _adfExplorer2_interpret_file_header(SEXP connection, SEXP vol_num, SEXP sectnum) {
  BEGIN_CPP11
    return cpp11::as_sexp(interpret_file_header(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num), cpp11::as_cpp<cpp11::decay_t<int>>(sectnum)));
  END_CPP11
}
// block_adf.cpp
list interpret_dir_header(SEXP connection, int vol_num, int sectnum);
extern "C" SEXP _adfExplorer2_interpret_dir_header(SEXP connection, SEXP vol_num, SEXP sectnum) {
  BEGIN_CPP11
    return cpp11::as_sexp(interpret_dir_header(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num), cpp11::as_cpp<cpp11::decay_t<int>>(sectnum)));
  END_CPP11
}
// block_adf.cpp
list interpret_root_header(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_interpret_root_header(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(interpret_root_header(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_format.cpp
SEXP adf_dev_format(SEXP connection, std::string name, bool ffs, bool intl, bool dircache, bool bootable);
extern "C" SEXP _adfExplorer2_adf_dev_format(SEXP connection, SEXP name, SEXP ffs, SEXP intl, SEXP dircache, SEXP bootable) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_dev_format(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(name), cpp11::as_cpp<cpp11::decay_t<bool>>(ffs), cpp11::as_cpp<cpp11::decay_t<bool>>(intl), cpp11::as_cpp<cpp11::decay_t<bool>>(dircache), cpp11::as_cpp<cpp11::decay_t<bool>>(bootable)));
  END_CPP11
}
// dev_info.cpp
std::string adf_dev_name(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_dev_name(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_dev_name(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
int adf_dev_size(SEXP connection);
extern "C" SEXP _adfExplorer2_adf_dev_size(SEXP connection) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_dev_size(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection)));
  END_CPP11
}
// dev_info.cpp
int adf_dev_nvol(SEXP connection);
extern "C" SEXP _adfExplorer2_adf_dev_nvol(SEXP connection) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_dev_nvol(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection)));
  END_CPP11
}
// dev_info.cpp
std::string adf_dev_type(SEXP connection);
extern "C" SEXP _adfExplorer2_adf_dev_type(SEXP connection) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_dev_type(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection)));
  END_CPP11
}
// dev_info.cpp
int adf_free_blocks(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_free_blocks(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_free_blocks(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
int adf_block_size(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_block_size(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_block_size(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
int adf_vol_size(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_vol_size(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_vol_size(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
bool adf_is_intl(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_is_intl(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_is_intl(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
bool adf_is_dircache(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_is_dircache(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_is_dircache(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
bool adf_is_ffs(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_is_ffs(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_is_ffs(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
bool adf_is_write_protected(SEXP connection);
extern "C" SEXP _adfExplorer2_adf_is_write_protected(SEXP connection) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_is_write_protected(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection)));
  END_CPP11
}
// dev_info.cpp
bool adf_is_bootable(SEXP connection, int vol_num);
extern "C" SEXP _adfExplorer2_adf_is_bootable(SEXP connection, SEXP vol_num) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_is_bootable(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num)));
  END_CPP11
}
// dev_info.cpp
bool adf_set_dev_name(SEXP connection, int vol_num, std::string new_name);
extern "C" SEXP _adfExplorer2_adf_set_dev_name(SEXP connection, SEXP vol_num, SEXP new_name) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_set_dev_name(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<int>>(vol_num), cpp11::as_cpp<cpp11::decay_t<std::string>>(new_name)));
  END_CPP11
}
// open_adf.cpp
SEXP open_adf_(std::string filename, bool write_protected);
extern "C" SEXP _adfExplorer2_open_adf_(SEXP filename, SEXP write_protected) {
  BEGIN_CPP11
    return cpp11::as_sexp(open_adf_(cpp11::as_cpp<cpp11::decay_t<std::string>>(filename), cpp11::as_cpp<cpp11::decay_t<bool>>(write_protected)));
  END_CPP11
}
// open_adf_file.cpp
SEXP adf_file_con_(SEXP connection, std::string filename, bool writable);
extern "C" SEXP _adfExplorer2_adf_file_con_(SEXP connection, SEXP filename, SEXP writable) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_file_con_(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection), cpp11::as_cpp<cpp11::decay_t<std::string>>(filename), cpp11::as_cpp<cpp11::decay_t<bool>>(writable)));
  END_CPP11
}
// open_adf_file.cpp
std::string adf_file_con_info(SEXP connection);
extern "C" SEXP _adfExplorer2_adf_file_con_info(SEXP connection) {
  BEGIN_CPP11
    return cpp11::as_sexp(adf_file_con_info(cpp11::as_cpp<cpp11::decay_t<SEXP>>(connection)));
  END_CPP11
}

extern "C" {
static const R_CallMethodDef CallEntries[] = {
    {"_adfExplorer2_adf_block_size",         (DL_FUNC) &_adfExplorer2_adf_block_size,         2},
    {"_adfExplorer2_adf_change_dir",         (DL_FUNC) &_adfExplorer2_adf_change_dir,         2},
    {"_adfExplorer2_adf_dev_format",         (DL_FUNC) &_adfExplorer2_adf_dev_format,         6},
    {"_adfExplorer2_adf_dev_name",           (DL_FUNC) &_adfExplorer2_adf_dev_name,           2},
    {"_adfExplorer2_adf_dev_nvol",           (DL_FUNC) &_adfExplorer2_adf_dev_nvol,           1},
    {"_adfExplorer2_adf_dev_size",           (DL_FUNC) &_adfExplorer2_adf_dev_size,           1},
    {"_adfExplorer2_adf_dev_type",           (DL_FUNC) &_adfExplorer2_adf_dev_type,           1},
    {"_adfExplorer2_adf_dir_exists_",        (DL_FUNC) &_adfExplorer2_adf_dir_exists_,        2},
    {"_adfExplorer2_adf_entry_info_",        (DL_FUNC) &_adfExplorer2_adf_entry_info_,        2},
    {"_adfExplorer2_adf_file_con_",          (DL_FUNC) &_adfExplorer2_adf_file_con_,          3},
    {"_adfExplorer2_adf_file_con_info",      (DL_FUNC) &_adfExplorer2_adf_file_con_info,      1},
    {"_adfExplorer2_adf_file_exists_",       (DL_FUNC) &_adfExplorer2_adf_file_exists_,       2},
    {"_adfExplorer2_adf_free_blocks",        (DL_FUNC) &_adfExplorer2_adf_free_blocks,        2},
    {"_adfExplorer2_adf_get_current_dir",    (DL_FUNC) &_adfExplorer2_adf_get_current_dir,    1},
    {"_adfExplorer2_adf_is_bootable",        (DL_FUNC) &_adfExplorer2_adf_is_bootable,        2},
    {"_adfExplorer2_adf_is_dircache",        (DL_FUNC) &_adfExplorer2_adf_is_dircache,        2},
    {"_adfExplorer2_adf_is_ffs",             (DL_FUNC) &_adfExplorer2_adf_is_ffs,             2},
    {"_adfExplorer2_adf_is_intl",            (DL_FUNC) &_adfExplorer2_adf_is_intl,            2},
    {"_adfExplorer2_adf_is_write_protected", (DL_FUNC) &_adfExplorer2_adf_is_write_protected, 1},
    {"_adfExplorer2_adf_mkdir",              (DL_FUNC) &_adfExplorer2_adf_mkdir,              2},
    {"_adfExplorer2_adf_path_to_entry",      (DL_FUNC) &_adfExplorer2_adf_path_to_entry,      3},
    {"_adfExplorer2_adf_remove_entry",       (DL_FUNC) &_adfExplorer2_adf_remove_entry,       3},
    {"_adfExplorer2_adf_set_dev_name",       (DL_FUNC) &_adfExplorer2_adf_set_dev_name,       3},
    {"_adfExplorer2_adf_set_entry_name_",    (DL_FUNC) &_adfExplorer2_adf_set_entry_name_,    3},
    {"_adfExplorer2_adf_upper",              (DL_FUNC) &_adfExplorer2_adf_upper,              2},
    {"_adfExplorer2_adf_vol_size",           (DL_FUNC) &_adfExplorer2_adf_vol_size,           2},
    {"_adfExplorer2_interpret_dir_header",   (DL_FUNC) &_adfExplorer2_interpret_dir_header,   3},
    {"_adfExplorer2_interpret_file_header",  (DL_FUNC) &_adfExplorer2_interpret_file_header,  3},
    {"_adfExplorer2_interpret_root_header",  (DL_FUNC) &_adfExplorer2_interpret_root_header,  2},
    {"_adfExplorer2_list_adf_entries_",      (DL_FUNC) &_adfExplorer2_list_adf_entries_,      4},
    {"_adfExplorer2_move_adf_internal",      (DL_FUNC) &_adfExplorer2_move_adf_internal,      3},
    {"_adfExplorer2_open_adf_",              (DL_FUNC) &_adfExplorer2_open_adf_,              2},
    {"_adfExplorer2_read_adf_block_",        (DL_FUNC) &_adfExplorer2_read_adf_block_,        2},
    {"_adfExplorer2_write_adf_block_",       (DL_FUNC) &_adfExplorer2_write_adf_block_,       3},
    {NULL, NULL, 0}
};
}

extern "C" attribute_visible void R_init_adfExplorer2(DllInfo* dll){
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  R_forceSymbols(dll, TRUE);
}

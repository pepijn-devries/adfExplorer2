#include "adf_file_info.h"

[[cpp11::register]]
SEXP adf_change_dir(SEXP connection, std::string path) {
  int mode = ADF_FI_EXPECT_DIR | ADF_FI_THROW_ERROR | ADF_FI_EXPECT_EXIST |
    ADF_FI_EXPECT_VALID_CHECKSUM;
  
  list entry = adf_path_to_entry(connection, path, mode);
  adf_change_dir_internal(
    connection,
    (SECTNUM)integers(entry["sector"]).at(0),
    integers(entry["volume"]).at(0)
  );
  return R_NilValue;
}

[[cpp11::register]]
SEXP adf_get_current_dir(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  int cur_vol = get_adf_vol_internal(connection);
  AdfVolume * vol = dev->volList[cur_vol];
  writable::list dev_l((R_xlen_t)0);
  dev_l.push_back(connection);
  writable::list result({
    "device"_nm = dev_l,
      "path"_nm = adf_entry_to_path(connection, cur_vol, vol->curDirPtr, TRUE)
  });
  return result;
}

list list_adf_entries3_(SEXP connection, AdfVolume * vol,
                        SECTNUM sector, int vol_num, bool recursive) {
  writable::list result;
  auto alist = new AdfList;
  auto entry = new AdfEntry;
  
  alist = adfGetRDirEnt ( vol, sector, FALSE );
  while ( alist ) {
    entry = (AdfEntry *)alist->content;
    
    result.push_back({
      "name"_nm = list()
    });
    writable::strings(result.attr("names")).at(result.size() - 1) = entry->name;
    alist = alist->next;
    if (entry->type == ST_DIR && recursive) {
      result[entry->name] =
        list_adf_entries3_(connection, vol, entry->sector, vol_num, recursive);
    }
    
  }
  adfFreeDirList(alist);
  delete alist;
  delete entry;
  return result;
}

list list_adf_entries2_(SEXP connection, AdfVolume * vol,
                        SECTNUM sector, int vol_num, bool recursive) {
  writable::list result;
  auto alist = new AdfList;
  auto entry = new AdfEntry;
  
  alist = adfGetRDirEnt ( vol, sector, FALSE );
  while ( alist ) {
    entry = (AdfEntry *)alist->content;
    
    result.push_back(
      writable::strings({
        adf_entry_to_path(connection, vol_num, entry->sector, TRUE)
      })
    );
    alist = alist->next;
    if (entry->type == ST_DIR && recursive) {
      result.push_back(
        list_adf_entries2_(connection, vol, entry->sector, vol_num, recursive)
      );
    }
    
  }
  adfFreeDirList(alist);
  delete alist;
  delete entry;
  return result;
}

[[cpp11::register]]
list list_adf_entries_(SEXP connection, std::string filename, bool recursive, bool nested) {
  writable::list result;
  AdfDevice * dev = get_adf_dev_internal(connection);
  
  int mode = ADF_FI_EXPECT_DIR | ADF_FI_THROW_ERROR | ADF_FI_EXPECT_EXIST |
    ADF_FI_EXPECT_VALID_CHECKSUM;
  list entry_pos = adf_path_to_entry(connection, filename, mode);
  int vol_num  = integers(entry_pos["volume"]).at(0);
  SECTNUM sect = integers(entry_pos["sector"]).at(0);
  
  if (vol_num < 0 || sect < (SECTNUM)0)
    Rf_error("Path does not exist");
  
  AdfVolume * vol = dev->volList[vol_num];
  if (nested) {
    result = list_adf_entries3_(connection, vol, sect, vol_num, recursive);
  } else {
    result = list_adf_entries2_(connection, vol, sect, vol_num, recursive);
  }
  return result;
}

[[cpp11::register]]
SEXP adf_mkdir(SEXP connection, std::string path) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  
  list entry = adf_path_to_entry(connection, path, 0);
  std::string remainder = strings(entry["remainder"]).at(0);
  int vol_num = integers(entry["volume"]).at(0);
  int sectype = integers(entry["header_sectype"]).at(0);
  if (sectype != ST_ROOT && sectype != ST_DIR)
    Rf_error("Parent of a new directory needs to be the root or another directory.");
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  
  int parent = integers(entry["parent"]).at(0);
  if (parent < vol->firstBlock || parent > vol->lastBlock) Rf_error("Invalid path");
  
  check_adf_name(remainder);
  RETCODE rc = adfCreateDir(vol, parent, remainder.c_str());
  if (rc != RC_OK) Rf_error("Failed to create directory '%s'.", remainder.c_str());
  return connection;
}

[[cpp11::register]]
SEXP adf_remove_entry(SEXP connection, std::string path, bool flush) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  
  int mode = ADF_FI_THROW_ERROR | ADF_FI_EXPECT_EXIST |
    ADF_FI_EXPECT_VALID_CHECKSUM;
  
  list entry    = adf_path_to_entry(connection, path, mode);
  int sectype   = integers(entry["header_sectype"]).at(0);
  if (sectype == ST_ROOT) Rf_error("Cannot remove a device's root");
  
  int vol_num   = integers(entry["volume"]).at(0);
  SECTNUM psect = integers(entry["parent"]).at(0);
  
  bool file_state = adf_check_file_state(dev, vol_num, integers(entry["sector"]).at(0));
  if (file_state)
    Rf_error("Cannot remove files with open connection. Close file first then try again.");
  
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  std::string name_s = (std::string)strings(entry["name"]).at(0);
  
  int nblocks = (vol->lastBlock - vol->firstBlock);
  writable::logicals block_bit((R_xlen_t)nblocks - 2);
  
  int i;
  // Store current state of bitmap if file needs to be flushed  
  if (flush) {
    for(i = vol->firstBlock + 2; i <= nblocks; i++) {
      block_bit[i - 2] = (r_bool)adfIsBlockFree(vol, i);
    }
  }
  
  const char * name = name_s.c_str();
  
  RETCODE rc = adfRemoveEntry(vol, psect, name);
  
  if (rc == RC_DIR_NOT_EMPTY) Rf_error("Can remove directory only when it is empty.");
  
  if (rc != RC_OK) Rf_error("Failed to remove entry from device.");
  
  writable::raws empty((R_xlen_t)512);
  for (i = 0; i < 512; i++) empty[i] = 0;
  
  // Erase blocks that have been set to free in the bitmap
  if (flush) {
    for (i = vol->firstBlock + 2; i <= nblocks; i++) {
      r_bool new_state = (r_bool)adfIsBlockFree(vol, i);
      r_bool old_state = (r_bool)block_bit[i - 2];
      if (!old_state && new_state) {
        write_adf_block_(connection, i, empty);
      }
    }
  }
  
  return R_NilValue;
}

[[cpp11::register]]
SEXP adf_set_entry_name_(SEXP connection, std::string path, std::string replacement) {
  int parent, vol_num, sectype;
  std::string name;
  RETCODE rc = RC_OK;
  if (Rf_inherits(connection, "adf_device")) {
    AdfDevice * dev = get_adf_dev_internal(connection);
    if (dev->readOnly) Rf_error("Cannot change entry name on 'readonly' device.");
    
    list entry = adf_path_to_entry(connection, path, 0);
    vol_num = integers(entry["volume"]).at(0);
    sectype = integers(entry["header_sectype"]).at(0);
    parent  = integers(entry["parent"]).at(0);
    name    = (std::string)strings(entry["name"]).at(0);
    check_volume_number(dev, vol_num);
    AdfVolume * vol = dev->volList[vol_num];
    if (sectype == ST_ROOT) {
      if (!adf_set_dev_name(connection, vol_num, replacement))
        rc = RC_ERROR;
    } else {
      rc =
        adfRenameEntry (vol, parent, name.c_str(), parent, replacement.c_str());
    }
  } else if (Rf_inherits(connection, "adf_file_con")) {
    Rconnection con = R_GetConnection(connection);
    if (!con->isopen) Rf_error("Connection is closed");
    AdfFile *af = (AdfFile *) con->private_ptr;
    int nl = af->fileHdr->nameLen;
    if (nl > MAXNAMELEN) nl = MAXNAMELEN;
    
    std::string entry_name =
      (std::string)(std::string(af->fileHdr->fileName).substr(0, nl));
    
    rc =
      adfRenameEntry (af->volume, af->fileHdr->parent,
                      entry_name.c_str(), af->fileHdr->parent,
                      replacement.c_str());
    if (rc == RC_OK) {
      af->fileHdr->nameLen = replacement.length();
      strcpy(af->fileHdr->fileName, replacement.c_str());
      // Don't think it is necesarry to update the checksum here
    }

  } else {
    Rf_error("`connection` should by of class `adf_device` or `adf_file_con`.");
  }
  if (rc != RC_OK) Rf_error("Failed to rename entry.");
  
  return connection;
}

[[cpp11::register]]
SEXP move_adf_internal(SEXP connection, std::string source, std::string destination) {
  if (source.size() != 1 && destination.size() != 1)
    Rf_error("`move_adf_internal` can only handle length 1 arguments");
  int mode = ADF_FI_THROW_ERROR | ADF_FI_EXPECT_EXIST |
    ADF_FI_EXPECT_VALID_CHECKSUM;
  
  list entry_src = adf_path_to_entry(connection, source, mode);
  list entry_dst = adf_path_to_entry(connection, destination, mode);
  writable::list entry_traverse;
  
  int sec_type_src = integers(entry_src["header_sectype"]).at(0);
  int sec_type_dst = integers(entry_dst["header_sectype"]).at(0);
  if (sec_type_src == ST_ROOT)
    Rf_error("Cannot move the root to elsewhere on the device.");
  if (sec_type_dst != ST_DIR && sec_type_dst != ST_ROOT)
    Rf_error("'destination' does not point at a directory.");
  
  int dst_check = integers(entry_dst["sector"]).at(0);
  int dst_parent = integers(entry_dst["parent"]).at(0);
  int src_check = integers(entry_src["sector"]).at(0);
  int cur_vol = integers(entry_src["volume"]).at(0);
  
  AdfDevice * dev = get_adf_dev_internal(connection);
  auto vol = dev->volList[cur_vol];
  
  // Check if the source header is not in the path of the destination
  uint8_t buf[512] = {0};
  bEntryBlock * entry = (bEntryBlock *) buf;
  
  bool test = (dst_check == src_check || dst_parent == src_check);
  
  int failsafe = 0L;
  while (sec_type_dst != ST_ROOT) {
    RETCODE rc = adfReadEntryBlock ( vol, dst_parent, entry );
    if (rc != RC_OK) Rf_error("Failed to check destination path");
    dst_parent = (int)entry->parent;
    sec_type_dst = (int)entry->secType;
    if (test || dst_parent == src_check)
      Rf_error("'source' is already in `destination` path, cannot move.");
    failsafe++;
    if (failsafe > 1000L) Rf_error("Unexpectedly deep path");
  }
  
  // Currently this function only checks if the move is allowed
  // It doesn't actually move anything
  return R_NilValue;
}

#include "file_info_adf.h"
#include "connect_adf.h"

r_string adf_upper(strings x, logicals intl) {
  if (x.size() != 1 || intl.size() != 1)
    Rf_error("`adf_upper` can only handle length 1 arguments");
  char* upper = new char[x.at(0).size() + 1];
  const char* input = ((std::string)x.at(0)).c_str();
  BOOL intl_b = intl.at(0) ? TRUE : FALSE;
  adfStrToUpper((uint8_t *)upper, (uint8_t *)input,
                x.at(0).size(), intl_b);
  r_string result = upper;
  delete[] upper;
  return result;
}

bool adf_check_volume(AdfDevice * dev, std::string vol_name,
                      int & cur_vol, int & cur_pos) {
  printf("TODO debug 1 %s\n", vol_name.c_str());
  if (vol_name.length() == 0) return false;
  printf("TODO debug 2\n");
  writable::strings vn({vol_name});
  for (int i = 0; i < dev->nVol; i++) {
    bool intl = isINTL(dev->volList[i]->dosType) == TRUE ||
      isDIRCACHE(dev->volList[i]->dosType) == TRUE;
    vn.at(0) = vol_name;
    std::string vol_name2 = (std::string)adf_upper(vn,
                             logicals({intl}));
    if (std::string("SYS").compare(vol_name2) == 0) {
      cur_pos = dev->volList[i]->rootBlock;
      return true;
    }
    std::string num_dev;
    if (dev->devType == DEVTYPE_FLOPDD ||
        dev->devType == DEVTYPE_FLOPHD) {
      num_dev = "DF" + std::to_string(i);
    } else if (dev->devType == DEVTYPE_HARDDISK ||
      dev->devType == DEVTYPE_HARDFILE) {
      num_dev = "DH" + std::to_string(i);
    }
    
    vn.at(0) = adf_dev_name_internal(dev, i);
    std::string dev_vol_name = (std::string)adf_upper(vn,
                                logicals({intl}));
    if (vol_name2.compare(dev_vol_name) == 0 ||
        num_dev.compare(vol_name2) == 0) {
      cur_vol = i;
      cur_pos = dev->volList[i]->rootBlock;
      return true;
    }
  }
  return false;
}

list adf_path_to_entry(
    SEXP connection, strings filename,
    int mode) {
  if (filename.size() != 1) Rf_error("Path can only be retrieved for single string.");
  writable::strings entry_name;
  entry_name.push_back(r_string(""));
  writable::list result({
    "volume"_nm = (int)-1,
    "sector"_nm = (int)-1,
    "header_sectype"_nm = (int)-1,
    "parent"_nm = (int)-1,
    "name"_nm = entry_name,
    "remainder"_nm = entry_name});
  
  if (filename.at(0) == R_NaString) return result;
  
  AdfDevice * dev = get_adf_dev_internal(connection);
  int cur_vol     = get_adf_vol_internal(connection);
  check_volume_number(dev, cur_vol);
  int parent      = -1;
  SECTNUM cur_pos = dev->volList[cur_vol]->curDirPtr;
  auto vol = dev->volList[cur_vol];
  
  std::stringstream ss((std::string)filename.at(0));
  
  if (((std::string)filename.at(0)).find(":") != std::string::npos) {
    std::string vol_name;
    std::getline(ss, vol_name, ':');
    if (!adf_check_volume(dev, vol_name, cur_vol, cur_pos)) {
      const char* message = "Could not find the specified volume on device";
      if ((mode & ADF_FI_THROW_ERROR) != 0) Rf_error("%s", message); else
        if ((mode & ADF_FI_WARN) != 0) Rf_warning("%s", message);
      return result;
    } else {
      vol = dev->volList[cur_vol];
    }
  }
  
  std::string path_chunk;
  
  uint8_t entry_buf[512] = {0};
  bEntryBlock * entry = (bEntryBlock *)entry_buf;
  
  const char* message1 = "No entry block for current dir.";
  RETCODE rc = adfReadEntryBlock ( vol, cur_pos, entry );
  if (rc != RC_OK) {
    if ((mode & ADF_FI_THROW_ERROR) != 0) Rf_error("%s", message1); else
      if ((mode & ADF_FI_WARN) != 0) Rf_warning("%s", message1);
    return result;
  }    
  while (std::getline(ss, path_chunk, '/')) {
    RETCODE rc = adfReadEntryBlock ( vol, cur_pos, entry );
    if (rc != RC_OK) {
      if ((mode & ADF_FI_THROW_ERROR) != 0) Rf_error("%s", message1); else
        if ((mode & ADF_FI_WARN) != 0) Rf_warning("%s", message1);
      return result;
    }
    parent = cur_pos;
    cur_pos = adfGetEntryByName ( vol, cur_pos, path_chunk.c_str(), entry );
    if (cur_pos < vol->firstBlock || cur_pos > vol->lastBlock) {
      const char* message = "Path not found.";
      if ((mode & (ADF_FI_EXPECT_EXIST | ADF_FI_THROW_ERROR)) != 0) {
        Rf_error("%s", message);
      } else {
        if ((mode & ADF_FI_WARN) != 0) Rf_warning("%s", message);
        result["remainder"] = writable::strings(r_string(path_chunk));
        break;
      }
    }
  }
  
  int nl = entry->nameLen;
  if (nl > MAXNAMELEN) nl = MAXNAMELEN;
  
  entry_name.at(0) = std::string(entry->name).substr(0, nl);
  int entry_sectype = entry->secType;
  
  result["name"]           = entry_name;
  result["volume"]         = as_sexp(cur_vol);
  result["sector"]         = as_sexp((int)cur_pos);
  result["header_sectype"] = as_sexp((int)entry_sectype);
  result["parent"]         = as_sexp((int)parent);
  

  if (((mode & ADF_FI_EXPECT_FILE) != 0) &&
      entry_sectype != ST_FILE) {
    const char * message = "Path does not point to a file";
    if ((mode & ADF_FI_THROW_ERROR) != 0)
      Rf_error("%s", message); else Rf_warning("%s", message);
  }
  if (((mode & ADF_FI_EXPECT_DIR) != 0) &&
      entry_sectype != ST_DIR && entry_sectype != ST_ROOT) {
    const char * message = "Path does not point to a directory";
    if ((mode & ADF_FI_THROW_ERROR) != 0)
      Rf_error("%s", message); else Rf_warning("%s", message);
  }
  
  return result;
}

r_string adf_entry_to_path(SEXP connection, int vol_num, int sectnum, bool full) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  
  uint8_t buf[512] = {0};
  bEntryBlock * entry = (bEntryBlock *) buf;
  
  std::string result("");
  int fail_safe = 0;
  int sn = sectnum;
  do {
    RETCODE rc = adfReadEntryBlock(vol, sn, entry);
    if (rc != RC_OK) Rf_error("Failed to read entry block");
    
    uint8_t namelen = entry->nameLen;
    if (namelen > MAXNAMELEN) {
      namelen = MAXNAMELEN;
      Rf_warning("Faulty entry name length. Entry name is truncated");
    }
    char name[MAXNAMELEN+3] = {0};
    memcpy(name, entry->name, namelen);
    std::string sep("");
    if (entry->secType == ST_DIR) sep = "/";
    if (entry->secType == ST_ROOT) sep = ":";
    result = std::string(name) + sep + result;
    fail_safe++;
    sn = entry->parent;
  } while (entry->secType != ST_ROOT && full && fail_safe < 1000);
  if (fail_safe == 1000) Rf_error("Unrealistically deep path");
  return r_string(result);
}

strings adf_dir_list(SEXP connection, strings filename) {
  writable::strings result;
  AdfDevice * dev = get_adf_dev_internal(connection);
  
  int mode = ADF_FI_EXPECT_DIR | ADF_FI_THROW_ERROR | ADF_FI_EXPECT_EXIST |
    ADF_FI_EXPECT_VALID_CHECKSUM;
  list entry_pos = adf_path_to_entry(connection, filename, mode);
  int vol_num  = integers(entry_pos["volume"]).at(0);
  SECTNUM sect = integers(entry_pos["sector"]).at(0);
  
  if (vol_num < 0 || sect < (SECTNUM)0) {
    Rf_error("Path does not exist");
  }
  AdfVolume * vol = dev->volList[vol_num];
  SECTNUM cur_pos = sect;
  auto list       = new AdfList;
  auto entry      = new AdfEntry;
  
  list = adfGetDirEnt ( vol, cur_pos );
  while ( list ) {
    entry = (AdfEntry *)list->content;
    result.push_back(entry->name);
    list = list->next;
  }
  
  adfFreeDirList(list);
  delete list;
  delete entry;
  return result;
}

SEXP adf_change_dir(SEXP connection, strings path) {
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

void adf_change_dir_internal(SEXP connection, SECTNUM sector, int volume) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, volume);
  AdfVolume * vol = dev->volList[volume];
  vol->curDirPtr = sector;
}

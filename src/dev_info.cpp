#include "dev_info.h"

void check_volume_number(AdfDevice *dev, int vol_num) {
  if (dev->nVol < 1) Rf_error("No volumes on device");
  if (vol_num < 0 || vol_num >= dev->nVol) Rf_error("Invalid volume number");
  return;
}

void check_adf_con (SEXP con) {
  if (! Rf_inherits(con, "adf_device"))
    Rf_error("Connection should inherit 'adf_device'.");
}

AdfContainer * getAC(Rconnection con) {
  AdfContainer *ac = (AdfContainer *)con->private_ptr;
  if (!ac) Rf_error("Lost pointer to AdfContainer.");
  return ac;
}

AdfDevice * get_adf_dev(Rconnection con) {
  AdfContainer *ac = getAC(con);
  if (!(ac->dev)) Rf_error("Lost pointer to AdfDevice.");
  return ac->dev;
}

AdfDevice * get_adf_dev_internal(SEXP con) {
  check_adf_con(con);
  Rconnection con2 = R_GetConnection(con);
  return (get_adf_dev(con2));
}

int get_adf_vol(Rconnection con) {
  AdfContainer *ac = getAC(con);
  return ac->currentVol;
}

int get_adf_vol_internal(SEXP con) {
  check_adf_con(con);
  Rconnection con2 = R_GetConnection(con);
  return get_adf_vol(con2);
}

void set_adf_vol(Rconnection con, int cur_vol) {
  AdfContainer *ac = getAC(con);
  ac->currentVol = cur_vol;
}

void set_adf_vol_internal(SEXP con, int cur_vol) {
  check_adf_con(con);
  Rconnection con2 = R_GetConnection(con);
  return set_adf_vol(con2, cur_vol);
}

std::string adf_dev_name_internal(AdfDevice * dev, int vol_num) {
  check_volume_number(dev, vol_num);
  std::string result = dev->volList[vol_num]->volName;
  return result;
}

[[cpp11::register]]
std::string adf_dev_name(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return(adf_dev_name_internal(dev, vol_num));
}

[[cpp11::register]]
int adf_dev_size(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return (int)dev->size;
}

[[cpp11::register]]
int adf_dev_nvol(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return (int)dev->nVol;
}

[[cpp11::register]]
std::string adf_dev_type(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  std::string result;
  
  switch (dev->devType) {
  case DEVTYPE_FLOPDD:
    result = "Floppy DD";
    break;
  case DEVTYPE_FLOPHD:
    result = "Floppy HD";
    break;
  case DEVTYPE_HARDDISK:
    result = "Hard disk";
    break;
  case DEVTYPE_HARDFILE:
    result = "Hard file";
    break;
  default:
    result = "Unknown device";
  break;
  }
  
  return result;
}

[[cpp11::register]]
int adf_free_blocks(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  return (int) adfCountFreeBlocks ( dev->volList[vol_num] );
}

[[cpp11::register]]
int adf_block_size(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  return (dev->volList[vol_num])->blockSize;
}

[[cpp11::register]]
int adf_vol_size(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  // Also count the boot block
  return (1 + vol->lastBlock - vol->firstBlock)*vol->blockSize;
}

[[cpp11::register]]
bool adf_is_intl(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  return (bool)isINTL(vol->dosType);
}

[[cpp11::register]]
bool adf_is_dircache(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  return (bool)isDIRCACHE(vol->dosType);
}

[[cpp11::register]]
bool adf_is_ffs(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  return (bool)isFFS(vol->dosType);
}

[[cpp11::register]]
bool adf_is_write_protected(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return dev->readOnly;
}

[[cpp11::register]]
bool adf_is_bootable(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  bool result = false;
  
  uint8_t buf[1024];
  RETCODE rc;
  if (dev->nVol < 1) {
    rc = adfReadBlockDev (dev, 0, 1024, buf);
  } else {
    AdfVolume * vol = dev->volList[vol_num];
    rc = adfReadBlock (vol, 0, buf);
    if (rc == RC_OK)
      rc = adfReadBlock (vol, 1, buf + LOGICAL_BLOCK_SIZE);
  }
  if (rc != RC_OK) Rf_error("Could not retrieve boot blocks");
  bBootBlock * boot = (bBootBlock *)buf;
  BOOL isdos =
    boot->dosType[0] == 'D' &&
    boot->dosType[1] == 'O' &&
    boot->dosType[2] == 'S';
  
  uint32_t boot_check = boot->checkSum;
#ifndef LITT_ENDIAN
  swLong((uint8_t *)&boot_check, boot->checkSum);
#endif
  
  uint32_t calc_checksum = adfBootSum(buf);
  
  result = (bool)isdos && calc_checksum == boot_check;
  return result;
}

[[cpp11::register]]
bool adf_set_dev_name(SEXP connection, int vol_num, std::string new_name) {
  int size = new_name.length();
  if (size > MAXNAMELEN) size = MAXNAMELEN;
  if (size == 0) Rf_error("New name should be at least 1 character long");
  AdfDevice * dev = get_adf_dev_internal(connection);
  if (dev->readOnly) Rf_error("Virtual device is read-only!");
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  
  uint8_t buf_root[512] = {0};
  bRootBlock * root = (bRootBlock *) buf_root;
  RETCODE rc = adfReadRootBlock(vol, vol->rootBlock, root);
  if (rc != RC_OK) Rf_error("Failed to read root block.");
  
  memset(root->diskName, 0, MAXNAMELEN);
  memcpy(root->diskName, new_name.c_str(), size);
  rc = adfWriteRootBlock(vol, vol->rootBlock, root);
  
  if (rc == RC_OK) {
    char * new_nm = (char*)malloc(size + 1);
    if (!new_nm) return false;
    memset(new_nm, 0, size + 1);
    memcpy(new_nm, new_name.c_str(), size);
    free(vol->volName);
    vol->volName = new_nm;
  } else {
    Rf_error("Failed to change virtual device name.");
  }
  return rc == RC_OK;
}

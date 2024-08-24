#include "connect_adf.h"
#include "dev_info.h"

void check_volume_number(AdfDevice *dev, int vol_num) {
  if (dev->nVol < 1) Rf_error("No volumes on device");
  if (vol_num < 0 || vol_num >= dev->nVol) Rf_error("Invalid volume number");
  return;
}

cpp11::r_string adf_dev_name_internal(AdfDevice * dev, int vol_num) {
  check_volume_number(dev, vol_num);
  cpp11::r_string result = (cpp11::r_string)dev->volList[vol_num]->volName;
  return result;
}

cpp11::r_string adf_dev_name(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return(adf_dev_name_internal(dev, vol_num));
}

int adf_dev_size(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return (int)dev->size;
}

int adf_dev_nvol(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return (int)dev->nVol;
}

cpp11::r_string adf_dev_type(SEXP connection) {
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
  
  return (cpp11::r_string)result;
}

int adf_free_blocks(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  return (int) adfCountFreeBlocks ( dev->volList[vol_num] );
}

int adf_block_size(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  return (dev->volList[vol_num])->blockSize;
}

int adf_vol_size(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  // Also count the boot block
  return (1 + vol->lastBlock - vol->firstBlock)*vol->blockSize;
}

cpp11::r_bool adf_is_intl(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  cpp11::r_bool result = isINTL(vol->dosType);
  return result;
}

cpp11::r_bool adf_is_dircache(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  cpp11::r_bool result = isDIRCACHE(vol->dosType);
  return result;
}

cpp11::r_bool adf_is_ffs(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  check_volume_number(dev, vol_num);
  AdfVolume * vol = dev->volList[vol_num];
  cpp11::r_bool result = isFFS(vol->dosType);
  return result;
}

cpp11::r_bool adf_is_write_protected(SEXP connection) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  return (cpp11::r_bool)dev->readOnly;
}

cpp11::r_bool adf_is_bootable(SEXP connection, int vol_num) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  cpp11::r_bool result = FALSE;
  
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

  result = isdos && calc_checksum == boot_check;
  return result;
}
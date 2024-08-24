#include "connect_adf.h"
#include "dev_format.h"

SEXP adf_dev_format(SEXP connection, SEXP name,
                    cpp11::logicals ffs, cpp11::logicals intl, cpp11::logicals dircache) {
  AdfDevice * dev = get_adf_dev_internal(connection);
  uint8_t boot_code[1024] = {0};
  uint8_t vol_type = 0;
  
  if (ffs.size() == 0 || ffs.size() > 1 || ffs.at(0) == NA_LOGICAL ||
      intl.size() == 0 || intl.size() > 1 || intl.at(0) == NA_LOGICAL ||
      dircache.size() == 0 || dircache.size() > 1 || dircache.at(0) == NA_LOGICAL)
    Rf_error("Logical arguments should have a length of one and not be NA");
  
  if (!(bool)intl.at(0) && (bool)dircache.at(0))
    Rf_warning("International mode is switched to TRUE as required for dircache mode");
  
  if ((bool)ffs.at(0)) vol_type |= FSMASK_FFS;
  // It seems that OS >=3 does not allow intl=T when dircache=T
  // when directory cache mode is set to TRUE, this is only allowed in 'international mode'.
  // The flag for the international mode is turned off however
  if ((bool)intl.at(0) && !(bool)dircache.at(0)) vol_type |= FSMASK_INTL;
  if ((bool)dircache.at(0)) vol_type |= FSMASK_DIRCACHE;
  
  if (dev->readOnly) Rf_error("Cannot format a write protected device");
  if (dev->nVol > 0) Rf_error("Cannot format a device with existing volumes");
  std::string name_s = (std::string)((cpp11::r_string)(cpp11::strings(name).at(0)));
  check_adf_name(name_s);
  const char * name_c = name_s.c_str();
  
  AdfVolume * vol = adfCreateVol(dev, 0, dev->cylinders, name_c, vol_type);
  if (vol == NULL) Rf_error("Failed to format device");

  if (dev->devType == DEVTYPE_FLOPDD || dev->devType == DEVTYPE_FLOPHD) {
    
    if (adfMountFlop(dev) != RC_OK ) Rf_error("Failed to mount floppy");
    set_adf_vol_internal(connection, 0);

  } else {

    if (adfMountHdFile(dev) != RC_OK || adfMountHdFileFix(dev) != RC_OK)
      Rf_error("Failed to mount harddisk");
    set_adf_vol_internal(connection, 0);

  }
  
  RETCODE rc;
  for (int i = 0; i < dev->nVol; i++) {
    AdfVolume * vol = adfMount(dev, i, dev->readOnly);
    rc = adfInstallBootBlock(vol, boot_code);
    rc = updateBootSum(vol);
    if (rc != RC_OK) Rf_error("Failed to install boot block");
  }

  return R_NilValue;
}


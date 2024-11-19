#' @keywords internal
"_PACKAGE"
NULL

.onUnload <- function(libpath) library.dynam.unload("adfExplorer2", libpath)

#' @useDynLib adfExplorer2, .registration = TRUE
#' @importFrom methods setOldClass
NULL

setOldClass("virtual_path")
setOldClass("adf_device")
setOldClass("adf_file_con")
setOldClass("adf_block")

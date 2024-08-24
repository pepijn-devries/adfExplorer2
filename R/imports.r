.onUnload <- function(libpath) library.dynam.unload("adfExplorer2", libpath)

#' @useDynLib adfExplorer2, .registration = TRUE
NULL

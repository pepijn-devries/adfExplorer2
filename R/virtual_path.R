#' A path pointing to a file or directory on a virtual ADF device
#' 
#' This function creates a path pointing to a file or directory
#' on a virtual ADF device (created with [`connect_adf()`] or [`create_adf_device()`]).
#' The virtual path created with this function can be used to establish a readable
#' or writable connection to a file, or obtain information about a file or directory.
#' See also `vignette("virtual_paths")`
#' @param dev A virtual ADF device (created with [`connect_adf()`] or
#' [`create_adf_device()`]).
#' Make sure a file system is present on the virtual device or install first when missing
#' using [`prepare_adf_device()`].
#' @param virtual_path A `character` string representing the path to a file or directory
#' on the virtual device.
#' @returns Returns a `virtual_path` class object.
#' @examples
#' adz_file <- system.file("example.adz", package = "adfExplorer2")
#' 
#' # Open a connection to a virtual device:
#' my_device <- connect_adf(adz_file)
#' 
#' # specify a virtual path:
#' my_path <- virtual_path(my_device, "DF0:s/startup-sequence")
#' 
#' # close the virtual device:
#' close(my_device)
#' @author Pepijn de Vries
#' @export
virtual_path <- function(dev, virtual_path) {
  if (!inherits(dev, "adf_device")) stop("`dev` should be of class `adf_device`.")
  if (typeof(virtual_path) != "character") stop("`virtual_path` should be of type `character`.")
  if (length(virtual_path) != 1) stop("Length of `virtual_path` should be 1.")
  if (is.na(virtual_path)) stop("`virtual_path` cannot be `NA`")
  result <- list(device = dev, path = virtual_path)
  class(result) <- union("virtual_path", class(result))
  result
}
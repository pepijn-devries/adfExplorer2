#' Test if an entry exists on a virtual device
#' 
#' Test if an entry (file or directory) exists on a virtual ADF device.
#' `adf_file_exists()` is the equivalent of [`file.exists()`] on a virtual ADF device.
#' `adf_dir_exists()` is the equivalent of [`dir.exists()`] on a virtual ADF device.
#' @param x Either a virtual device or virtual path.
#' @param virtual_path A [`virtual_path()`] to be checked for existence. Should
#' be omitted when `x` is already a virtual path.
#' @param ... Ignored
#' @returns `adf_file_exists()` returns `TRUE` if the path exists on the virtual device, `FALSE` otherwise.
#' `adf_dir_exists()` returns `TRUE` when the path exists and is a directory, `FALSE` otherwise.
#' @examples
#' ## First setup a connection to a virtual device
#' adz_file <- system.file("example.adz", package = "adfExplorer2")
#' my_device <- connect_adf(adz_file)
#' 
#' adf_file_exists(my_device, "s/startup-sequence")
#' adf_dir_exists(my_device, "s/startup-sequence")
#' 
#' close(my_device)
#' @author Pepijn de Vries
#' @rdname exists
#' @export
adf_file_exists <- function(x, virtual_path, ...) {
  UseMethod("adf_file_exists", x)
}

#' @rdname exists
#' @export adf_file_exists.adf_device
#' @export
adf_file_exists.adf_device <- function(x, virtual_path,...) {
  if (length(virtual_path) != 1) stop("Cannot test more than 1 virtual path per call")
  if (typeof(virtual_path) != "character") {
    .check_dev(x, virtual_path)
    virtual_path <- unclass(virtual_path)$path
  }
  adf_file_exists_(x, virtual_path)
}

#' @rdname exists
#' @export adf_file_exists.virtual_path
#' @export
adf_file_exists.virtual_path <- function(x, virtual_path,...) {
  if (!missing(virtual_path))
    stop("'virtual_path' argument should be omitted when 'x' is already a 'virtual_path'.")
  x <- unclass(x)
  adf_file_exists_(x$device[[1]], x$path)
}

#' @rdname exists
#' @export
adf_dir_exists <- function(x, virtual_path, ...) {
  UseMethod("adf_dir_exists", x)
}

#' @rdname exists
#' @export adf_dir_exists.adf_device
#' @export
adf_dir_exists.adf_device <- function(x, virtual_path,...) {
  if (length(virtual_path) != 1) stop("Cannot test more than 1 virtual path per call")
  if (typeof(virtual_path) != "character") {
    .check_dev(x, virtual_path)
    virtual_path <- unclass(virtual_path)$path
  }
  adf_dir_exists_(x, virtual_path)
}

#' @rdname exists
#' @export adf_dir_exists.virtual_path
#' @export
adf_dir_exists.virtual_path <- function(x, virtual_path,...) {
  if (!missing(virtual_path))
    stop("'virtual_path' argument should be omitted when 'x' is already a 'virtual_path'.")
  x <- unclass(x)
  adf_dir_exists_(x$device[[1]], x$path)
}
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

#' Retrieve information from entry headers on virtual ADF devices
#' 
#' Retrieve information from entry (file and directory) headers on virtual ADF devices.
#' Get information like entry name, modification date, file size etc.
#' @inheritParams adf_file_exists
#' @returns Returns a named list of entry properties. Elements included in the list
#' depend on the type of entry (root, directory or file).
#' @examples
#' ## First setup a connection to a virtual device
#' adz_file <- system.file("example.adz", package = "adfExplorer2")
#' my_device <- connect_adf(adz_file)
#' 
#' adf_entry_info(my_device, "DF0:")
#' adf_entry_info(my_device, "s")
#' adf_entry_info(my_device, "s/startup-sequence")
#' 
#' close(my_device)
#' @author Pepijn de Vries
#' @rdname entry_info
#' @export
adf_entry_info <- function(x, virtual_path, ...) {
  UseMethod("adf_entry_info", x)
}

#' @rdname entry_info
#' @export adf_entry_info.adf_device
#' @method adf_entry_info adf_device
#' @export
adf_entry_info.adf_device <- function(x, virtual_path, ...) {
  UseMethod("adf_entry_info.adf_device", virtual_path)
}

#' @rdname entry_info
#' @name adf_entry_info
#' @method adf_entry_info.adf_device virtual_path
#' @export
adf_entry_info.adf_device.virtual_path <- function(x, virtual_path, ...) {
  if (length(virtual_path) > 1) {
    lapply(virtual_path, \(y) adf_entry_info(x, y, ...))
  } else {
    .check_dev(x, virtual_path)
    virtual_path <- unclass(virtual_path)$path
    adf_entry_info(x, virtual_path)
  }
}

#' @rdname entry_info
#' @name adf_entry_info
#' @method adf_entry_info.adf_device character
#' @export
adf_entry_info.adf_device.character <- function(x, virtual_path, ...) {
  adf_entry_info_(x, virtual_path)
}

#' @rdname entry_info
#' @export adf_entry_info.virtual_path
#' @export
adf_entry_info.virtual_path <- function(x, virtual_path, ...) {
  if (!missing(virtual_path)) stop("`virtual_path` only needs to be specified when `x` is an `adf_device`.")
  if (length(x) > 1) lapply(x, adf_entry_info, virtual_path, ...) else {
    x <- unclass(x)
    adf_entry_info(x$device[[1]], x$path)
  }
}

#' @rdname entry_info
#' @export adf_entry_info.adf_file_con
#' @export
adf_entry_info.adf_file_con <- function(x, virtual_path, ...) {
  if (!missing(virtual_path)) stop("`virtual_path` only needs to be specified when `x` is an `adf_device`.")
  adf_entry_info_(x, "")
}

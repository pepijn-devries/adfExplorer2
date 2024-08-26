#' Changing and creating directories on a virtual device
#' 
#' `adf_directory()` shows the current directory of a virtual device, when a file
#' system is present. When connecting to or creating a new device, the current
#' directory is the disk's root by default. To change the current directory,
#' use `adf_directory()` in combination with the assign operator (`<-`).
#' 
#' To create a new directory on a device use `adf_make_dir()` and use a full or
#' relative path name to specify the new directory name.
#' 
#' See `vignette("virtual_path)` for a note on file and directory names on the Amiga.
#' @inheritParams device_capacity
#' @param value A `character` string or a `virtual_path` (see [`virtual_path()`])
#' representing directory you wish to set as current.
#' @param virtual_path A `character` string or a `virtual_path` (see [`virtual_path()`])
#' specifying the name of the new directory to be created.
#' @returns `adf_make_dir()` returns the device connection. `adf_directory()` returns
#' the current directory as a `virtual_path` class object.
#' @examples
#' ## First setup a connection to a virtual device
#' adz_file <- system.file("example.adz", package = "adfExplorer2")
#' my_device <- connect_adf(adz_file)
#' 
#' ## Show the current directory
#' adf_directory(my_device)
#' 
#' ## Create a new directory
#' adf_make_dir(my_device, "DF0:s/newdir")
#' 
#' ## Change the current dir to the new directory:
#' adf_directory(my_device) <- "DF0:s/newdir"
#' 
#' ## Close the virtual device
#' close(my_device)
#' @rdname adf_directory
#' @author Pepijn de Vries
#' @export
adf_directory <- function(dev, ...) {
  UseMethod("adf_directory", dev)
}

#' @rdname adf_directory
#' @name adf_directory
#' @export
adf_directory.adf_device <- function(dev, ...) {
  adf_get_current_dir(dev)
}

#' @rdname adf_directory
#' @export
`adf_directory<-` <- function(dev, ..., value) {
  UseMethod("adf_directory<-", dev)
}

#' @rdname adf_directory
#' @name adf_directory<-
#' @method adf_directory<- adf_device
#' @export adf_directory<-.adf_device
#' @aliases adf_directory.adf_device<-
#' @aliases adf_directory<-.adf_device
#' @export
`adf_directory.adf_device<-` <- `adf_directory<-.adf_device` <- function(dev, ..., value) {
  UseMethod("adf_directory.adf_device<-", value)
}

#' @rdname adf_directory
#' @name adf_directory<-
#' @method adf_directory.adf_device<- character
#' @export
`adf_directory.adf_device<-.character` <- function(dev, ..., value) {
  adf_change_dir(dev, value)
  dev
}

#' @rdname adf_directory
#' @name adf_directory<-
#' @method adf_directory.adf_device<- virtual_path
#' @export
`adf_directory.adf_device<-.virtual_path` <- function(dev, ..., value) {
  if (!identical(dev, value$device))
    stop("Cannot change directoy to a path on a different device")
  adf_change_dir(dev, value$path)
  dev
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @export
adf_make_dir <- function(dev, virtual_path, ...) {
  UseMethod("adf_make_dir")
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir adf_device
#' @export adf_make_dir.adf_device
#' @export
adf_make_dir.adf_device <- function(dev, virtual_path, ...) {
  UseMethod("adf_make_dir.adf_device", virtual_path)
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir.adf_device character
#' @export
adf_make_dir.adf_device.character <- function(dev, virtual_path, ...) {
  adf_mkdir(dev, virtual_path)
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir.adf_device virtual_path
#' @export
adf_make_dir.adf_device.virtual_path <- function(dev, virtual_path, ...) {
  if (!identical(dev, virtual_path$device))
    stop("Cannot make a directoy based on a path on a different device")
  adf_make_dir(dev, virtual_path$path, ...)
}

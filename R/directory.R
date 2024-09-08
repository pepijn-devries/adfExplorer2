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
#' See `vignette("virtual_paths")` for a note on file and directory names on the Amiga.
#' @inheritParams device_capacity
#' @param x An `adf_device` or `virtual_path` class object. The first specifies
#' the device on which a directory needs to be created. The latter specifies
#' both the directory and the device on which it needs to be created.
#' @param value A `character` string or a `virtual_path` (see [`virtual_path()`])
#' representing directory you wish to set as current.
#' @param virtual_path A `character` string or a `virtual_path` (see [`virtual_path()`])
#' specifying the name of the new directory to be created. Should be missing when `x` is
#' of class `virtual_path`
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
#' @include helpers.R
#' @author Pepijn de Vries
#' @export
adf_directory <- function(dev, ...) {
  UseMethod("adf_directory", dev)
}

#' @rdname adf_directory
#' @name adf_directory
#' @export
adf_directory.adf_device <- function(dev, ...) {
  vctrs::new_rcrd(
    adf_get_current_dir(dev),
    class = "virtual_path")
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
  .check_dev(dev, value)
  adf_change_dir(dev, value$path)
  dev
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @export
adf_make_dir <- function(x, virtual_path, ...) {
  UseMethod("adf_make_dir")
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir adf_device
#' @export adf_make_dir.adf_device
#' @export
adf_make_dir.adf_device <- function(x, virtual_path, ...) {
  UseMethod("adf_make_dir.adf_device", virtual_path)
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir virtual_path
#' @export adf_make_dir.virtual_path
#' @export
adf_make_dir.virtual_path <- function(x, virtual_path, ...) {
  if (!missing(virtual_path))
    stop("`virtual_path` argument should be missing when `x` is of class `virtual_path`.")
  if (length(x) != 1)
    stop("Cannot create multiple virtual paths at once")
  x <- unclass(x)
  adf_make_dir(x$device[[1]], x$path, ...)
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir.adf_device character
#' @export
adf_make_dir.adf_device.character <- function(x, virtual_path, ...) {
  adf_mkdir(x, virtual_path)
}

#' @rdname adf_directory
#' @name adf_make_dir
#' @method adf_make_dir.adf_device virtual_path
#' @export
adf_make_dir.adf_device.virtual_path <- function(x, virtual_path, ...) {
  .check_dev(x, virtual_path())
  adf_make_dir(x, virtual_path$path, ...)
}

#' List entries in a directory of a virtual ADF device
#' 
#' Get an overview of all entries (files and directories) in a specific
#' directory.
#' @param x Either an `adf_device` class object, in which case the `virtual_path`
#' argument needs to be specified; or, a `virtual_path` class object.
#' @param virtual_path The virtual path for which you wish to obtain a list
#' of entries (see also `vignette("virtual_paths")`). When missing,
#' entries for the current directory ([`adf_directory()`]) are returned, wen
#' `x` is an `adf_device` class object. If `x` is a `virtual_path` class
#' object, content of the path defined in that object is listed
#' @param recursive A `logical` value. When set to `TRUE`, the function is
#' called recursively for all subdirectories in `virtual_path`.
#' @param ... Ignored
#' @returns A vector of `virtual_path` class objects
#' @examples
#' ## First setup a connection to a virtual device
#' adz_file <- system.file("example.adz", package = "adfExplorer2")
#' my_device <- connect_adf(adz_file)
#' 
#' ## List all entries in the disk's root:
#' adf_dir_list(my_device)
#' ## List all entries on the disk:
#' adf_dir_list(my_device, recursive = TRUE)
#'
#' close(my_device)
#' @rdname adf_dir_list
#' @name adf_dir_list
#' @author Pepijn de Vries
#' @export
adf_dir_list <- function(x, virtual_path, recursive = FALSE, ...) {
  UseMethod("adf_dir_list", x)
}

#' @rdname adf_dir_list
#' @method adf_dir_list adf_device
#' @export adf_dir_list.adf_device
#' @export
adf_dir_list.adf_device <- function(x, virtual_path, recursive = FALSE, ...) {
  if (missing(virtual_path)) {
    adf_dir_list(x, adf_directory(x), recursive = recursive, ...)
  } else UseMethod("adf_dir_list.adf_device", virtual_path)
}

#' @rdname adf_dir_list
#' @method adf_dir_list virtual_path
#' @export adf_dir_list.virtual_path
#' @export
adf_dir_list.virtual_path <- function(x, virtual_path, recursive = FALSE, ...) {
  if (!missing(virtual_path))
    stop("`virtual_path` argument should be missing when `x` is of class `virtual_path`.")
  if (length(x) != 1)
    stop("Cannot list directory entries for multiple virtual paths")
  x <- unclass(x)
  adf_dir_list(x$device[[1]], x$path, recursive = recursive, ...)
}

#' @rdname adf_dir_list
#' @name adf_dir_list
#' @method adf_dir_list.adf_device character
#' @export
adf_dir_list.adf_device.character <- function(x, virtual_path, recursive = FALSE, ...) {
  result <- adf_dir_list_(x, virtual_path, recursive) |> unlist()
  vctrs::new_rcrd(list(device = rep(list(x), length(result)),
                       path   = result),
                  class = "virtual_path")
}

#' @rdname adf_dir_list
#' @name adf_dir_list
#' @method adf_dir_list.adf_device virtual_path
#' @export
adf_dir_list.adf_device.virtual_path <- function(x, virtual_path,
                                                 recursive = FALSE, ...) {
  if (length(virtual_path) != 1)
    stop("Cannot list directory entries for multiple virtual paths")
  .check_dev(x, virtual_path)
  virtual_path <- unclass(virtual_path[[1]])
  adf_dir_list(x, virtual_path$path, recursive = recursive, ...)
}
#TODO setup generics and specifics for pseudo connections (adf_file_con)

#' @export
close.adf_file_con <- function(con, ...) {
  adf_close_file_con(con) |> invisible()
}

#' @export
close.adf_device <- function(con, ...) {
  close_adf(con) |> invisible()
}

#' @export
seek.adf_file_con <- function(con, where = NA, origin = "start", ...) {
  
  origin <- pmatch(origin, c("start", "current", "end"))
  seek_adf(con, as.numeric(where), origin)
}

#' @export
readBin <-
  function(con, what, n = 1L, size = NA_integer_, signed = TRUE,
           endian = .Platform$endian) {
    UseMethod("readBin")
  }

#' @export
readBin.default <-
  function(con, what, n = 1L, size = NA_integer_, signed = TRUE,
           endian = .Platform$endian) {
    base::readBin(con, what, n, size, signed, endian)
  }

#' @export
readBin.adf_file_con <-
  function(con, what, n = 1L, size = NA_integer_, signed = TRUE,
           endian = .Platform$endian) {
    w <- c("numeric", "double", "integer", "int", "logical",
           "complex", "character", "raw")
    if(!is.character(what) || is.na(what) ||
       length(what) != 1L || ## hence length(what) == 1:
       !any(what == w))
      what <- typeof(what)
    what <- pmatch(what, w)
    if (!endian %in% c("big", "little", "swap"))
      stop("invalid 'endian' argument")
    swap <- endian != .Platform$endian
    adf_readbin(con, what, as.integer(n), as.integer(size), as.logical(signed), swap)
  }

#' @export
readLines <-
  function(con, n = -1L, ok = TRUE, warn = TRUE,
           encoding = "unknown", skipNul = FALSE) {
    UseMethod("readLines")
  }

#' @export
readLines.default <-
  function(con = stdin(), n = -1L, ok = TRUE, warn = TRUE,
           encoding = "unknown", skipNul = FALSE) {
    base::readBin(con, n, ok, warn, encoding, skipNul)
  }

#' @export
readLines.adf_file_con <-
  function(con, n = -1L, ok = TRUE, warn = TRUE,
           encoding = "unknown", skipNul = FALSE) {
    adf_readlines(con, n, ok, warn, encoding, skipNul)
  }

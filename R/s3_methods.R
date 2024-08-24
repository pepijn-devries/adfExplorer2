#' Basic methods for S3 class objects
#' 
#' Format and print methods for all S3 class objects created with `adfExplorer2`
#' 
#' @param x Object to be formated or printed
#' @param ... Ignored or passed on to next methods
#' @rdname s3_methods
#' @export format.adf_device
#' @export
format.adf_device <- function(x, ...) {
  nvol   <- n_volumes(x)
  result <- paste0(
    ifelse(is_bootable(x), "Bootable ", "Non-bootable "),
    ifelse(is_write_protected(x), "write protected ", ""),
    ifelse(nvol < 1L, "unformatted ", "DOS "),
    device_type(x)
  )
  if (nvol > 0L) {
    for (i in (seq_len(nvol) - 1)) {
      perc_full <- 100L*(1L - (bytes_free(x, i) / volume_capacity(x, i)))
      result <- sprintf("%s\n  Volume %i [%s%s%s]: %s (%.1f%%)",
                        result, i,
                        ifelse(is_fast_file_system(x), "f", "-"),
                        ifelse(is_international(x), "i", "-"),
                        ifelse(is_dircache(x), "d", "-"),
                        volume_name(x, i), perc_full)
    }
  }
  result
}

#' @rdname s3_methods
#' @export format.adf_file_con
#' @export
format.adf_file_con <- function(x, ...) {
  adf_file_con_info(x)
}

#' @rdname s3_methods
#' @export format.adf_block
#' @export
format.adf_block <- function(x, ...) {
  .displayRawData(x, ...)
}

#' @rdname s3_methods
#' @export format.virtual_path
#' @export
format.virtual_path <- function(x, ...) {
  entry <- adf_path_to_entry(x$device, x$path, 0)
  if (entry$sector < 0) return ("Invalid path")
  info <-
    switch(
      as.character(entry$header_sectype),
      `-3` = interpret_file_header(x$device, entry$volume, entry$sector),
      `2`  = interpret_dir_header(x$device, entry$volume, entry$sector),
      `1`  = interpret_root_header(x$device, entry$volume))
  size <- sprintf("%5.1f kB", info$byteSize/1024)
  if (length(size) == 0) size <- strrep(" ", 8)
  if (is.null(info$access) || !is.logical(info$access))
    access <- strrep(" ", 8) else
    access <-
    ifelse(info$access, names(info$access), "-") |> paste(collapse = "")
  sprintf("%-4s %s %s %s",
          info$secType,
          size,
          access,
          info[grepl("NAME", toupper(names(info)))])
}

#' @rdname s3_methods
#' @export
print.adf_device <- function(x, ...) {
  cat(format(x, ...))
  cat("\n")
}

#' @rdname s3_methods
#' @export
print.adf_file_con <- function(x, ...) {
  cat(format(x, ...))
  cat("\n")
}

#' @rdname s3_methods
#' @export
print.adf_block <- function(x, ...) {
  cat(paste(format(x, ...), collapse = "\n"))
}

#' @rdname s3_methods
#' @export
print.virtual_path <- function(x, ...) {
  cat(format(x, ...))
  cat("\n")
}

.displayRawData <- function(x, ncol = 4, col.wid = 4, address.len = 3, hex.upper = T) {
  nrow <- ceiling(length(x) / (ncol*col.wid))
  len  <- nrow*ncol*col.wid
  x   <- c(x, raw(len - length(x)))
  m   <- matrix(x, nrow, ncol*col.wid, byrow = T)
  hex <- apply(m, 1, function (x) paste0(sprintf("%02x", as.numeric(x)), collapse = ""))
  hex <- unlist(lapply(hex, function (x)
    paste0(substring(x,
                     seq(1, (2*ncol*col.wid-1), 2*col.wid),
                     seq(1, (2*ncol*col.wid-1), 2*col.wid) + 2*col.wid - 1),
           collapse = " ")))
  if (hex.upper) hex <- toupper(hex)
  ch  <- apply(m, 1, .rawToCharDot)
  add <- sprintf(paste0("%0", address.len, "x"), (0:(length(ch) - 1))*ncol*col.wid)
  if (hex.upper) add <- toupper(add)
  add <- paste0("0x", add)
  m   <- apply(cbind(add, hex, ch), 1, paste0, collapse = "  ")
  return(invisible(m))
}

.rawToCharDot <- function(raw_dat) {
  raw_dat[raw_dat <= as.raw(0x1F)] <- as.raw(46)
  raw_dat[raw_dat >= as.raw(0x21) & raw_dat <= as.raw(0x25)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0x81)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0x8d)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0x8f)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0x90)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0x9d)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0xad)] <- as.raw(46)
  raw_dat[raw_dat == as.raw(0x7f)] <- as.raw(46)
  
  return(iconv(rawToChar(raw_dat), from = "ISO-8859-1", to = "UTF-8"))
}
adz_file <- system.file("example.adz", package = "adfExplorer2")

# ---------------- adfExplorer
results1 <- rep(0, 100)
for (i in seq_along(results1)) {
  start1 <- Sys.time()
  library(adfExplorer)
  my_disk1 <- read.adz(adz_file)
  current.adf.dir(my_disk1)
  current.adf.dir(my_disk1) <- "s"
  get.adf.file(my_disk1, "startup-sequence") |>
    rawToChar()
  rm(my_disk1)
  end1 <- Sys.time()
  results1[i] <- end1 - start1
}

# ---------------- adfExplorer2
results2 <- rep(0, 100)
for (i in seq_along(results2)) {
  start2 <- Sys.time()
  library(adfExplorer2)
  my_disk2 <- connect_adf(adz_file)
  adf_directory(my_disk2)
  adf_directory(my_disk2) <- "s"
  con <- adf_file_con(my_disk2, "startup-sequence")
  readLines(con, warn = FALSE)
  close(con)
  close(my_disk2)
  rm(my_disk2)
  end2 <- Sys.time()
  results2[i] <- end2 - start2
}

bench_result <-
  data.frame(
    test = c("adfExplorer", "adfExplorer2"),
    bench_mean = c(mean(results1), mean(results2)),
    bench_sd = c(sd(results1), sd(results2))
  )

save(bench_result, file = "inst/benchresult.rdata", compress = TRUE)

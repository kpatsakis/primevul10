String HHVM_FUNCTION(socket_strerror,
                     int errnum) {
  /*
   * PHP5 encodes both the h_errno and errno values into a single int:
   * < -10000: transformed h_errno value
   * >= -10000: errno value
   */
  if (errnum < -10000) {
    errnum = (-errnum) - 10000;
#if HAVE_HSTRERROR
    return String(hstrerror(errnum), CopyString);
#endif
    return folly::format("Host lookup error {}", errnum).str();
  }

  return String(folly::errnoStr(errnum));
}
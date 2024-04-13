static bool get_pcre_fullinfo(pcre_cache_entry* pce) {
  pcre_extra extra;
  init_local_extra(&extra, pce->extra);

  /* Calculate the size of the offsets array*/
  int rc = pcre_fullinfo(pce->re, &extra, PCRE_INFO_CAPTURECOUNT,
                         &pce->num_subpats);
  if (rc < 0) {
    raise_warning("Internal pcre_fullinfo() error %d", rc);
    return false;
  }
  pce->num_subpats++;
  return true;
}
inspect_with_augeas (guestfs_h *g, struct inspect_fs *fs,
                     const char **configfiles,
                     int (*f) (guestfs_h *, struct inspect_fs *))
{
  /* Security: Refuse to do this if a config file is too large. */
  for (const char **i = configfiles; *i != NULL; i++) {
    if (guestfs_exists(g, *i) == 0) continue;

    int64_t size = guestfs_filesize (g, *i);
    if (size == -1)
      /* guestfs_filesize failed and has already set error in handle */
      return -1;
    if (size > MAX_AUGEAS_FILE_SIZE) {
      error (g, _("size of %s is unreasonably large (%" PRIi64 " bytes)"),
             *i, size);
      return -1;
    }
  }

  /* If !feature_available (g, "augeas") then the next call will fail.
   * Arguably we might want to fall back to a non-Augeas method in
   * this case.
   */
  if (guestfs_aug_init (g, "/", 16|32) == -1)
    return -1;

  int r = -1;

  /* Tell Augeas to only load one file (thanks Raphaël Pinson). */
#define AUGEAS_LOAD "/augeas/load//incl[. != \""
#define AUGEAS_LOAD_LEN (strlen(AUGEAS_LOAD))
  size_t conflen = strlen(configfiles[0]);
  size_t buflen = AUGEAS_LOAD_LEN + conflen + 1 /* Closing " */;
  CLEANUP_FREE char *buf =
    safe_malloc (g, buflen + 2 /* Closing ] + null terminator */);

  memcpy(buf, AUGEAS_LOAD, AUGEAS_LOAD_LEN);
  memcpy(buf + AUGEAS_LOAD_LEN, configfiles[0], conflen);
  buf[buflen - 1] = '"';
#undef AUGEAS_LOAD_LEN
#undef AUGEAS_LOAD

#define EXCL " and . != \""
#define EXCL_LEN (strlen(EXCL))
  for (const char **i = &configfiles[1]; *i != NULL; i++) {
    size_t orig_buflen = buflen;
    conflen = strlen(*i);
    buflen += EXCL_LEN + conflen + 1 /* Closing " */;
    buf = safe_realloc(g, buf, buflen + 2 /* Closing ] + null terminator */);
    char *s = buf + orig_buflen;

    memcpy(s, EXCL, EXCL_LEN);
    memcpy(s + EXCL_LEN, *i, conflen);
    buf[buflen - 1] = '"';
  }
#undef EXCL_LEN
#undef EXCL

  buf[buflen] = ']';
  buf[buflen + 1] = '\0';

  if (guestfs_aug_rm (g, buf) == -1)
    goto out;

  if (guestfs_aug_load (g) == -1)
    goto out;

  r = f (g, fs);

 out:
  guestfs_aug_close (g);

  return r;
}
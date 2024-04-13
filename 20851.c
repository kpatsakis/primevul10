rename_if_dos_device_name (char *file_name)
{
  /* We could have a file whose name is a device on MS-DOS.  Trying to
   * retrieve such a file would fail at best and wedge us at worst.  We need
   * to rename such files. */
  char *base;
  struct stat st_buf;
  char fname[PATH_MAX];

  strcpy (fname, file_name);
  base = basename (fname);
  if (((stat(base, &st_buf)) == 0) && (S_ISCHR(st_buf.st_mode))) {
    size_t blen = strlen (base);

    /* Prepend a '_'.  */
    memmove (base + 1, base, blen + 1);
    base[0] = '_';
    strcpy (file_name, fname);
  }
  return file_name;
}
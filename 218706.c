guestfs___check_freebsd_root (guestfs_h *g, struct inspect_fs *fs)
{
  fs->type = OS_TYPE_FREEBSD;

  /* FreeBSD has no authoritative version file.  The version number is
   * in /etc/motd, which the system administrator might edit, but
   * we'll use that anyway.
   */

  if (guestfs_exists (g, "/etc/motd") > 0) {
    if (parse_release_file (g, fs, "/etc/motd") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }

  /* Determine the architecture. */
  check_architecture (g, fs);

  /* We already know /etc/fstab exists because it's part of the test above. */
  const char *configfiles[] = { "/etc/fstab", NULL };
  if (inspect_with_augeas (g, fs, configfiles, check_fstab) == -1)
    return -1;

  /* Determine hostname. */
  if (check_hostname_unix (g, fs) == -1)
    return -1;

  return 0;
}
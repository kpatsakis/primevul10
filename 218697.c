guestfs___check_netbsd_root (guestfs_h *g, struct inspect_fs *fs)
{

  if (guestfs_exists (g, "/etc/release") > 0) {
    char *major, *minor;
    if (parse_release_file (g, fs, "/etc/release") == -1)
      return -1;

    if (match2 (g, fs->product_name, re_netbsd, &major, &minor)) {
      fs->type = OS_TYPE_NETBSD;
      fs->major_version = guestfs___parse_unsigned_int (g, major);
      free (major);
      if (fs->major_version == -1) {
        free (minor);
        return -1;
      }
      fs->minor_version = guestfs___parse_unsigned_int (g, minor);
      free (minor);
      if (fs->minor_version == -1)
        return -1;
    }
  } else {
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
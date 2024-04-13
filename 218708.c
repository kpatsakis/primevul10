guestfs___check_hurd_root (guestfs_h *g, struct inspect_fs *fs)
{
  fs->type = OS_TYPE_HURD;

  if (guestfs_exists (g, "/etc/debian_version") > 0) {
    fs->distro = OS_DISTRO_DEBIAN;

    if (parse_release_file (g, fs, "/etc/debian_version") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }

  /* Arch Hurd also exists, but inconveniently it doesn't have
   * the normal /etc/arch-release file.  XXX
   */

  /* Determine the architecture. */
  check_architecture (g, fs);

  /* XXX Check for /etc/fstab. */

  /* Determine hostname. */
  if (check_hostname_unix (g, fs) == -1)
    return -1;

  return 0;
}
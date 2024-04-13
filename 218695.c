check_hostname_unix (guestfs_h *g, struct inspect_fs *fs)
{
  switch (fs->type) {
  case OS_TYPE_LINUX:
  case OS_TYPE_HURD:
    /* Red Hat-derived would be in /etc/sysconfig/network or
     * /etc/hostname (RHEL 7+, F18+).  Debian-derived in the file
     * /etc/hostname.  Very old Debian and SUSE use /etc/HOSTNAME.
     * It's best to just look for each of these files in turn, rather
     * than try anything clever based on distro.
     */
    if (guestfs_is_file (g, "/etc/HOSTNAME")) {
      fs->hostname = guestfs___first_line_of_file (g, "/etc/HOSTNAME");
      if (fs->hostname == NULL)
        return -1;
      if (STREQ (fs->hostname, "")) {
        free (fs->hostname);
        fs->hostname = NULL;
      }
    }

    if (!fs->hostname && guestfs_is_file (g, "/etc/hostname")) {
      fs->hostname = guestfs___first_line_of_file (g, "/etc/hostname");
      if (fs->hostname == NULL)
        return -1;
      if (STREQ (fs->hostname, "")) {
        free (fs->hostname);
        fs->hostname = NULL;
      }
    }

    if (!fs->hostname && guestfs_is_file (g, "/etc/sysconfig/network")) {
      const char *configfiles[] = { "/etc/sysconfig/network", NULL };
      if (inspect_with_augeas (g, fs, configfiles,
                               check_hostname_redhat) == -1)
        return -1;
    }
    break;

  case OS_TYPE_FREEBSD:
  case OS_TYPE_NETBSD:
    /* /etc/rc.conf contains the hostname, but there is no Augeas lens
     * for this file.
     */
    if (guestfs_is_file (g, "/etc/rc.conf")) {
      if (check_hostname_freebsd (g, fs) == -1)
        return -1;
    }
    break;

  case OS_TYPE_WINDOWS: /* not here, see check_windows_system_registry */
  case OS_TYPE_DOS:
  case OS_TYPE_OPENBSD:
  case OS_TYPE_UNKNOWN:
    /* nothing */;
  }

  return 0;
}
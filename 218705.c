guestfs___check_linux_root (guestfs_h *g, struct inspect_fs *fs)
{
  int r;

  fs->type = OS_TYPE_LINUX;

  if (guestfs_exists (g, "/etc/lsb-release") > 0) {
    r = parse_lsb_release (g, fs);
    if (r == -1)        /* error */
      return -1;
    if (r == 1)         /* ok - detected the release from this file */
      goto skip_release_checks;
  }

  if (guestfs_exists (g, "/etc/redhat-release") > 0) {
    fs->distro = OS_DISTRO_REDHAT_BASED; /* Something generic Red Hat-like. */

    if (parse_release_file (g, fs, "/etc/redhat-release") == -1)
      return -1;

    char *major, *minor;
    if ((major = match1 (g, fs->product_name, re_fedora)) != NULL) {
      fs->distro = OS_DISTRO_FEDORA;
      fs->major_version = guestfs___parse_unsigned_int (g, major);
      free (major);
      if (fs->major_version == -1)
        return -1;
    }
    else if (match2 (g, fs->product_name, re_rhel_old, &major, &minor) ||
             match2 (g, fs->product_name, re_rhel, &major, &minor)) {
      fs->distro = OS_DISTRO_RHEL;
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
    else if ((major = match1 (g, fs->product_name, re_rhel_no_minor)) != NULL) {
      fs->distro = OS_DISTRO_RHEL;
      fs->major_version = guestfs___parse_unsigned_int (g, major);
      free (major);
      if (fs->major_version == -1)
        return -1;
      fs->minor_version = 0;
    }
    else if (match2 (g, fs->product_name, re_centos_old, &major, &minor) ||
             match2 (g, fs->product_name, re_centos, &major, &minor)) {
      fs->distro = OS_DISTRO_CENTOS;
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
    else if ((major = match1 (g, fs->product_name, re_centos_no_minor)) != NULL) {
      fs->distro = OS_DISTRO_CENTOS;
      fs->major_version = guestfs___parse_unsigned_int (g, major);
      free (major);
      if (fs->major_version == -1)
        return -1;
      fs->minor_version = 0;
    }
    else if (match2 (g, fs->product_name, re_scientific_linux_old, &major, &minor) ||
             match2 (g, fs->product_name, re_scientific_linux, &major, &minor)) {
      fs->distro = OS_DISTRO_SCIENTIFIC_LINUX;
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
    else if ((major = match1 (g, fs->product_name, re_scientific_linux_no_minor)) != NULL) {
      fs->distro = OS_DISTRO_SCIENTIFIC_LINUX;
      fs->major_version = guestfs___parse_unsigned_int (g, major);
      free (major);
      if (fs->major_version == -1)
        return -1;
      fs->minor_version = 0;
    }
  }
  else if (guestfs_exists (g, "/etc/debian_version") > 0) {
    fs->distro = OS_DISTRO_DEBIAN;

    if (parse_release_file (g, fs, "/etc/debian_version") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }
  else if (guestfs_exists (g, "/etc/pardus-release") > 0) {
    fs->distro = OS_DISTRO_PARDUS;

    if (parse_release_file (g, fs, "/etc/pardus-release") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }
  else if (guestfs_exists (g, "/etc/arch-release") > 0) {
    fs->distro = OS_DISTRO_ARCHLINUX;

    /* /etc/arch-release file is empty and I can't see a way to
     * determine the actual release or product string.
     */
  }
  else if (guestfs_exists (g, "/etc/gentoo-release") > 0) {
    fs->distro = OS_DISTRO_GENTOO;

    if (parse_release_file (g, fs, "/etc/gentoo-release") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }
  else if (guestfs_exists (g, "/etc/meego-release") > 0) {
    fs->distro = OS_DISTRO_MEEGO;

    if (parse_release_file (g, fs, "/etc/meego-release") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }
  else if (guestfs_exists (g, "/etc/slackware-version") > 0) {
    fs->distro = OS_DISTRO_SLACKWARE;

    if (parse_release_file (g, fs, "/etc/slackware-version") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }
  else if (guestfs_exists (g, "/etc/ttylinux-target") > 0) {
    fs->distro = OS_DISTRO_TTYLINUX;

    if (parse_release_file (g, fs, "/etc/ttylinux-target") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }
  else if (guestfs_exists (g, "/etc/SuSE-release") > 0) {
    fs->distro = OS_DISTRO_SUSE_BASED;

    if (parse_suse_release (g, fs, "/etc/SuSE-release") == -1)
      return -1;

  }
  /* Buildroot (http://buildroot.net) is an embedded Linux distro
   * toolkit.  It is used by specific distros such as Cirros.
   */
  else if (guestfs_exists (g, "/etc/br-version") > 0) {
    if (guestfs_exists (g, "/usr/share/cirros/logo") > 0)
      fs->distro = OS_DISTRO_CIRROS;
    else
      fs->distro = OS_DISTRO_BUILDROOT;

    /* /etc/br-version has the format YYYY.MM[-git/hg/svn release] */
    if (parse_release_file (g, fs, "/etc/br-version") == -1)
      return -1;

    if (guestfs___parse_major_minor (g, fs) == -1)
      return -1;
  }

 skip_release_checks:;

  /* Determine the architecture. */
  check_architecture (g, fs);

  /* We already know /etc/fstab exists because it's part of the test
   * for Linux root above.  We must now parse this file to determine
   * which filesystems are used by the operating system and how they
   * are mounted.
   */
  const char *configfiles[] = { "/etc/fstab", "/etc/mdadm.conf", NULL };
  if (inspect_with_augeas (g, fs, configfiles, check_fstab) == -1)
    return -1;

  /* Determine hostname. */
  if (check_hostname_unix (g, fs) == -1)
    return -1;

  return 0;
}
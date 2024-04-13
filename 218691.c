compile_regexps (void)
{
  const char *err;
  int offset;

#define COMPILE(re,pattern,options)                                     \
  do {                                                                  \
    re = pcre_compile ((pattern), (options), &err, &offset, NULL);      \
    if (re == NULL) {                                                   \
      ignore_value (write (2, err, strlen (err)));                      \
      abort ();                                                         \
    }                                                                   \
  } while (0)

  COMPILE (re_fedora, "Fedora release (\\d+)", 0);
  COMPILE (re_rhel_old,
           "Red Hat.*release (\\d+).*Update (\\d+)", 0);
  COMPILE (re_rhel,
           "Red Hat.*release (\\d+)\\.(\\d+)", 0);
  COMPILE (re_rhel_no_minor,
           "Red Hat.*release (\\d+)", 0);
  COMPILE (re_centos_old,
           "CentOS.*release (\\d+).*Update (\\d+)", 0);
  COMPILE (re_centos,
           "CentOS.*release (\\d+)\\.(\\d+)", 0);
  COMPILE (re_centos_no_minor,
           "CentOS.*release (\\d+)", 0);
  COMPILE (re_scientific_linux_old,
           "Scientific Linux.*release (\\d+).*Update (\\d+)", 0);
  COMPILE (re_scientific_linux,
           "Scientific Linux.*release (\\d+)\\.(\\d+)", 0);
  COMPILE (re_scientific_linux_no_minor,
           "Scientific Linux.*release (\\d+)", 0);
  COMPILE (re_major_minor, "(\\d+)\\.(\\d+)", 0);
  COMPILE (re_xdev, "^/dev/(h|s|v|xv)d([a-z]+)(\\d*)$", 0);
  COMPILE (re_cciss, "^/dev/(cciss/c\\d+d\\d+)(?:p(\\d+))?$", 0);
  COMPILE (re_mdN, "^(/dev/md\\d+)$", 0);
  COMPILE (re_freebsd, "^/dev/ad(\\d+)s(\\d+)([a-z])$", 0);
  COMPILE (re_diskbyid, "^/dev/disk/by-id/.*-part(\\d+)$", 0);
  COMPILE (re_netbsd, "^NetBSD (\\d+)\\.(\\d+)", 0);
  COMPILE (re_opensuse, "^(openSUSE|SuSE Linux|SUSE LINUX) ", 0);
  COMPILE (re_sles, "^SUSE (Linux|LINUX) Enterprise ", 0);
  COMPILE (re_nld, "^Novell Linux Desktop ", 0);
  COMPILE (re_opensuse_version, "^VERSION = (\\d+)\\.(\\d+)", 0);
  COMPILE (re_sles_version, "^VERSION = (\\d+)", 0);
  COMPILE (re_sles_patchlevel, "^PATCHLEVEL = (\\d+)", 0);
}
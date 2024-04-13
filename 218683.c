free_regexps (void)
{
  pcre_free (re_fedora);
  pcre_free (re_rhel_old);
  pcre_free (re_rhel);
  pcre_free (re_rhel_no_minor);
  pcre_free (re_centos_old);
  pcre_free (re_centos);
  pcre_free (re_centos_no_minor);
  pcre_free (re_scientific_linux_old);
  pcre_free (re_scientific_linux);
  pcre_free (re_scientific_linux_no_minor);
  pcre_free (re_major_minor);
  pcre_free (re_xdev);
  pcre_free (re_cciss);
  pcre_free (re_mdN);
  pcre_free (re_freebsd);
  pcre_free (re_diskbyid);
  pcre_free (re_netbsd);
  pcre_free (re_opensuse);
  pcre_free (re_sles);
  pcre_free (re_nld);
  pcre_free (re_opensuse_version);
  pcre_free (re_sles_version);
  pcre_free (re_sles_patchlevel);
}
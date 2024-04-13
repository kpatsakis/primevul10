check_hostname_redhat (guestfs_h *g, struct inspect_fs *fs)
{
  char *hostname;

  /* Errors here are not fatal (RHBZ#726739), since it could be
   * just missing HOSTNAME field in the file.
   */
  guestfs_push_error_handler (g, NULL, NULL);
  hostname = guestfs_aug_get (g, "/files/etc/sysconfig/network/HOSTNAME");
  guestfs_pop_error_handler (g);

  /* This is freed by guestfs___free_inspect_info.  Note that hostname
   * could be NULL because we ignored errors above.
   */
  fs->hostname = hostname;
  return 0;
}
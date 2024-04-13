add_fstab_entry (guestfs_h *g, struct inspect_fs *fs,
                 const char *mountable, const char *mountpoint)
{
  /* Add this to the fstab entry in 'fs'.
   * Note these are further filtered by guestfs_inspect_get_mountpoints
   * and guestfs_inspect_get_filesystems.
   */
  size_t n = fs->nr_fstab + 1;
  struct inspect_fstab_entry *p;

  p = realloc (fs->fstab, n * sizeof (struct inspect_fstab_entry));
  if (p == NULL) {
    perrorf (g, "realloc");
    return -1;
  }

  fs->fstab = p;
  fs->nr_fstab = n;

  /* These are owned by the handle and freed by guestfs___free_inspect_info. */
  fs->fstab[n-1].mountable = safe_strdup (g, mountable);
  fs->fstab[n-1].mountpoint = safe_strdup (g, mountpoint);

  debug (g, "fstab: mountable=%s mountpoint=%s", mountable, mountpoint);

  return 0;
}
check_fstab (guestfs_h *g, struct inspect_fs *fs)
{
  CLEANUP_FREE_STRING_LIST char **entries = NULL;
  char **entry;
  char augpath[256];
  CLEANUP_HASH_FREE Hash_table *md_map;

  /* Generate a map of MD device paths listed in /etc/mdadm.conf to MD device
   * paths in the guestfs appliance */
  if (map_md_devices (g, &md_map) == -1) return -1;

  entries = guestfs_aug_match (g, "/files/etc/fstab/*[label() != '#comment']");
  if (entries == NULL)
    return -1;

  if (entries[0] == NULL) {
    error (g, _("could not parse /etc/fstab or empty file"));
    return -1;
  }

  for (entry = entries; *entry != NULL; entry++) {
    snprintf (augpath, sizeof augpath, "%s/spec", *entry);
    CLEANUP_FREE char *spec = guestfs_aug_get (g, augpath);
    if (spec == NULL)
      return -1;

    /* Ignore /dev/fd (floppy disks) (RHBZ#642929) and CD-ROM drives. */
    if ((STRPREFIX (spec, "/dev/fd") && c_isdigit (spec[7])) ||
        STREQ (spec, "/dev/floppy") ||
        STREQ (spec, "/dev/cdrom"))
      continue;

    snprintf (augpath, sizeof augpath, "%s/file", *entry);
    CLEANUP_FREE char *mp = guestfs_aug_get (g, augpath);
    if (mp == NULL)
      return -1;

    /* Ignore certain mountpoints. */
    if (STRPREFIX (mp, "/dev/") ||
        STREQ (mp, "/dev") ||
        STRPREFIX (mp, "/media/") ||
        STRPREFIX (mp, "/proc/") ||
        STREQ (mp, "/proc") ||
        STRPREFIX (mp, "/selinux/") ||
        STREQ (mp, "/selinux") ||
        STRPREFIX (mp, "/sys/") ||
        STREQ (mp, "/sys"))
      continue;

    /* Resolve UUID= and LABEL= to the actual device. */
    CLEANUP_FREE char *mountable = NULL;
    if (STRPREFIX (spec, "UUID="))
      mountable = guestfs_findfs_uuid (g, &spec[5]);
    else if (STRPREFIX (spec, "LABEL="))
      mountable = guestfs_findfs_label (g, &spec[6]);
    /* Ignore "/.swap" (Pardus) and pseudo-devices like "tmpfs". */
    else if (STREQ (spec, "/dev/root"))
      /* Resolve /dev/root to the current device. */
      mountable = safe_strdup (g, fs->mountable);
    else if (STRPREFIX (spec, "/dev/"))
      /* Resolve guest block device names. */
      mountable = resolve_fstab_device (g, spec, md_map);

    /* If we haven't resolved the device successfully by this point,
     * we don't care, just ignore it.
     */
    if (mountable == NULL)
      continue;

    snprintf (augpath, sizeof augpath, "%s/vfstype", *entry);
    CLEANUP_FREE char *vfstype = guestfs_aug_get (g, augpath);
    if (vfstype == NULL) return -1;

    if (STREQ (vfstype, "btrfs")) {
      snprintf (augpath, sizeof augpath, "%s/opt", *entry);
      CLEANUP_FREE_STRING_LIST char **opts = guestfs_aug_match (g, augpath);
      if (opts == NULL) return -1;

      for (char **opt = opts; *opt; opt++) {
        CLEANUP_FREE char *optname = guestfs_aug_get (g, augpath);
        if (optname == NULL) return -1;

        if (STREQ (optname, "subvol")) {
          snprintf (augpath, sizeof augpath, "%s/value", *opt);
          CLEANUP_FREE char *subvol = guestfs_aug_get (g, augpath);
          if (subvol == NULL) return -1;

          char *new = safe_asprintf (g, "btrfsvol:%s/%s", mountable, subvol);
          free (mountable);
          mountable = new;
        }
      }
    }

    if  (add_fstab_entry (g, fs, mountable, mp) == -1) return -1;
  }

  return 0;
}
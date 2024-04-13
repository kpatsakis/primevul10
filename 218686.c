resolve_fstab_device_xdev (guestfs_h *g, const char *type, const char *disk,
                           const char *part, char **device_ret)
{
  CLEANUP_FREE char *name = NULL;
  char *device;
  CLEANUP_FREE_STRING_LIST char **devices = NULL;
  size_t i, count;
  struct drive *drv;
  const char *p;

  /* type: (h|s|v|xv)
   * disk: ([a-z]+)
   * part: (\d*)
   */

  devices = guestfs_list_devices (g);
  if (devices == NULL)
    return -1;

  /* Check any hints we were passed for a non-heuristic mapping */
  name = safe_asprintf (g, "%sd%s", type, disk);
  ITER_DRIVES (g, i, drv) {
    if (drv->name && STREQ (drv->name, name)) {
      device = safe_asprintf (g, "%s%s", devices[i], part);
      if (!is_partition (g, device)) {
        free (device);
        return 0;
      }
      *device_ret = device;
      break;
    }
  }

  /* Guess the appliance device name if we didn't find a matching hint */
  if (!*device_ret) {
    /* Count how many disks the libguestfs appliance has */
    for (count = 0; devices[count] != NULL; count++)
      ;

    /* Calculate the numerical index of the disk */
    i = disk[0] - 'a';
    for (p = disk + 1; *p != '\0'; p++) {
      i += 1; i *= 26;
      i += *p - 'a';
    }

    /* Check the index makes sense wrt the number of disks the appliance has.
     * If it does, map it to an appliance disk.
     */
    if (i < count) {
      device = safe_asprintf (g, "%s%s", devices[i], part);
      if (!is_partition (g, device)) {
        free (device);
        return 0;
      }
      *device_ret = device;
    }
  }

  return 0;
}
resolve_fstab_device_cciss (guestfs_h *g, const char *disk, const char *part,
                            char **device_ret)
{
  char *device;
  CLEANUP_FREE_STRING_LIST char **devices = NULL;
  size_t i;
  struct drive *drv;

  /* disk: (cciss/c\d+d\d+)
   * part: (\d+)?
   */

  devices = guestfs_list_devices (g);
  if (devices == NULL)
    return -1;

  /* Check any hints we were passed for a non-heuristic mapping */
  ITER_DRIVES (g, i, drv) {
    if (drv->name && STREQ (drv->name, disk)) {
      if (part) {
        device = safe_asprintf (g, "%s%s", devices[i], part);
        if (!is_partition (g, device)) {
          free (device);
          return 0;
        }
        *device_ret = device;
      }
      else
        *device_ret = safe_strdup (g, devices[i]);
      break;
    }
  }

  /* We don't try to guess mappings for cciss devices */
  return 0;
}
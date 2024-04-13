resolve_fstab_device_diskbyid (guestfs_h *g, const char *part,
                               char **device_ret)
{
  int nr_devices;
  char *device;

  /* For /dev/disk/by-id there is a limit to what we can do because
   * original SCSI ID information has likely been lost.  This
   * heuristic will only work for guests that have a single block
   * device.
   *
   * So the main task here is to make sure the assumptions above are
   * true.
   *
   * XXX Use hints from virt-p2v if available.
   * See also: https://bugzilla.redhat.com/show_bug.cgi?id=836573#c3
   */

  nr_devices = guestfs_nr_devices (g);
  if (nr_devices == -1)
    return -1;

  /* If #devices isn't 1, give up trying to translate this fstab entry. */
  if (nr_devices != 1)
    return 0;

  /* Make the partition name and check it exists. */
  device = safe_asprintf (g, "/dev/sda%s", part);
  if (!is_partition (g, device)) {
    free (device);
    return 0;
  }

  *device_ret = device;
  return 0;
}
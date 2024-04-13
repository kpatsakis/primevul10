is_partition (guestfs_h *g, const char *partition)
{
  CLEANUP_FREE char *device = NULL;

  guestfs_push_error_handler (g, NULL, NULL);

  if ((device = guestfs_part_to_dev (g, partition)) == NULL) {
    guestfs_pop_error_handler (g);
    return 0;
  }

  if (guestfs_device_index (g, device) == -1) {
    guestfs_pop_error_handler (g);
    return 0;
  }

  guestfs_pop_error_handler (g);

  return 1;
}
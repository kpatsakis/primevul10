parse_release_file (guestfs_h *g, struct inspect_fs *fs,
                    const char *release_filename)
{
  fs->product_name = guestfs___first_line_of_file (g, release_filename);
  if (fs->product_name == NULL)
    return -1;
  if (STREQ (fs->product_name, "")) {
    free (fs->product_name);
    fs->product_name = NULL;
    error (g, _("release file %s is empty or malformed"), release_filename);
    return -1;
  }
  return 0;
}
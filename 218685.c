map_app_md_devices (guestfs_h *g, Hash_table **map)
{
  CLEANUP_FREE_STRING_LIST char **mds = NULL;
  size_t n = 0;

  /* A hash mapping uuids to md device names */
  *map = hash_initialize(16, NULL, uuid_hash, uuid_cmp, md_uuid_free);
  if (*map == NULL) g->abort_cb();

  mds = guestfs_list_md_devices(g);
  if (mds == NULL) goto error;

  for (char **md = mds; *md != NULL; md++) {
    CLEANUP_FREE_STRING_LIST char **detail = guestfs_md_detail (g, *md);
    if (detail == NULL) goto error;

    /* Iterate over keys until we find uuid */
    char **i;
    for (i = detail; *i != NULL; i += 2) {
      if (STREQ(*i, "uuid")) break;
    }

    /* We found it */
    if (*i) {
      /* Next item is the uuid value */
      i++;

      md_uuid *entry = safe_malloc(g, sizeof(md_uuid));
      entry->path = safe_strdup(g, *md);

      if (parse_uuid(*i, entry->uuid) == -1) {
        /* Invalid UUID is weird, but not fatal. */
        debug(g, "inspect-os: guestfs_md_detail returned invalid "
                 "uuid for %s: %s", *md, *i);
        md_uuid_free(entry);
        continue;
      }

      const void *matched = NULL;
      switch (hash_insert_if_absent(*map, entry, &matched)) {
        case -1:
          g->abort_cb();

        case 0:
          /* Duplicate uuid in for md device is weird, but not fatal. */
          debug(g, "inspect-os: md devices %s and %s have the same uuid",
                ((md_uuid *)matched)->path, entry->path);
          md_uuid_free(entry);
          break;

        default:
          n++;
      }
    }
  }

  return n;

error:
  hash_free (*map); *map = NULL;

  return -1;
}
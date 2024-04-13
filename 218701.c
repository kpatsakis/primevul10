map_md_devices(guestfs_h *g, Hash_table **map)
{
  CLEANUP_HASH_FREE Hash_table *app_map = NULL;
  CLEANUP_FREE_STRING_LIST char **matches = NULL;
  ssize_t n_app_md_devices;

  *map = NULL;

  /* Get a map of md device uuids to their device names in the appliance */
  n_app_md_devices = map_app_md_devices (g, &app_map);
  if (n_app_md_devices == -1) goto error;

  /* Nothing to do if there are no md devices */
  if (n_app_md_devices == 0)
    return 0;

  /* Get all arrays listed in mdadm.conf */
  matches = guestfs_aug_match(g, "/files/etc/mdadm.conf/array");
  if (!matches) goto error;

  /* Log a debug message if we've got md devices, but nothing in mdadm.conf */
  if (matches[0] == NULL) {
    debug(g, "Appliance has MD devices, but augeas returned no array matches "
             "in mdadm.conf");
    return 0;
  }

  *map = hash_initialize(16, NULL, mdadm_app_hash, mdadm_app_cmp,
                                   mdadm_app_free);
  if (!*map) g->abort_cb();

  for (char **m = matches; *m != NULL; m++) {
    /* Get device name and uuid for each array */
    CLEANUP_FREE char *dev_path = safe_asprintf (g, "%s/devicename", *m);
    char *dev = guestfs_aug_get (g, dev_path);
    if (!dev) goto error;

    CLEANUP_FREE char *uuid_path = safe_asprintf (g, "%s/uuid", *m);
    CLEANUP_FREE char *uuid = guestfs_aug_get (g, uuid_path);
    if (!uuid) {
      free (dev);
      continue;
    }

    /* Parse the uuid into an md_uuid structure so we can look it up in the
     * uuid->appliance device map */
    md_uuid mdadm;
    mdadm.path = dev;
    if (parse_uuid(uuid, mdadm.uuid) == -1) {
      /* Invalid uuid. Weird, but not fatal. */
      debug(g, "inspect-os: mdadm.conf contains invalid uuid for %s: %s",
            dev, uuid);
      free (dev);
      continue;
    }

    /* If there's a corresponding uuid in the appliance, create a new
     * entry in the transitive map */
    md_uuid *app = hash_lookup(app_map, &mdadm);
    if (app) {
      mdadm_app *entry = safe_malloc(g, sizeof(mdadm_app));
      entry->mdadm = dev;
      entry->app = safe_strdup(g, app->path);

      switch (hash_insert_if_absent(*map, entry, NULL)) {
        case -1:
          g->abort_cb();

        case 0:
          /* Duplicate uuid in for md device is weird, but not fatal. */
          debug(g, "inspect-os: mdadm.conf contains multiple entries for %s",
                app->path);
          mdadm_app_free(entry);
          continue;
      }
    } else
      free (dev);
  }

  return 0;

error:
  if (*map) hash_free (*map);

  return -1;
}
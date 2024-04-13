cdio_add_device_list(char **device_list[], const char *drive,
                     unsigned int *num_drives)
{
  if (NULL != drive) {
    unsigned int j;
    char real_device_1[PATH_MAX];
    char real_device_2[PATH_MAX];
    cdio_realpath(drive, real_device_1);
    /* Check if drive is already in list. */
    for (j=0; j<*num_drives; j++) {
      cdio_realpath((*device_list)[j], real_device_2);
      if (strcmp(real_device_1, real_device_2) == 0) break;
    }

    if (j==*num_drives) {
      /* Drive not in list. Add it. */
      (*num_drives)++;
      *device_list = realloc(*device_list, (*num_drives) * sizeof(char *));
      cdio_debug("Adding drive %s to list of devices", drive);
      (*device_list)[*num_drives-1] = strdup(drive);
      }

  } else {
    (*num_drives)++;
    if (*device_list) {
      *device_list = realloc(*device_list, (*num_drives) * sizeof(char *));
    } else {
      *device_list = malloc((*num_drives) * sizeof(char *));
    }
    cdio_debug("Adding NULL to end of drive list of size %d", (*num_drives)-1);
    (*device_list)[*num_drives-1] = NULL;
  }
}
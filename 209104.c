int el_build_index(LOGBOOK *lbs, BOOL rebuild)
/* scan all ??????a.log files and build an index table in eli[] */
{
   char *file_list, error_str[256], base_dir[256], *buffer;
   int index, n;
   int i, status;
   unsigned char digest[16];

   if (rebuild) {
      xfree(lbs->el_index);
      xfree(lbs->n_el_index);
   }

   lbs->n_el_index = xmalloc(sizeof(int));
   *lbs->n_el_index = 0;
   lbs->el_index = xmalloc(0);

   /* get data directory */
   strcpy(base_dir, lbs->data_dir);

   if (get_verbose() >= VERBOSE_DEBUG) {
      /* show MD5 from config file */

      load_config_section(lbs->name, &buffer, error_str);
      if (error_str[0])
         eprintf(error_str);
      else {
         remove_crlf(buffer);
         MD5_checksum(buffer, strlen(buffer), digest);
         eprintf("\n\nConfig [%s],                           MD5=", lbs->name);

         for (i = 0; i < 16; i++)
            eprintf("%02X", digest[i]);
         eprintf("\n\n");
      }
      if (buffer)
         xfree(buffer);
   }

   if (get_verbose() >= VERBOSE_DEBUG)
      eprintf("Entries:\n");

   // move files to directories if (new layout to reduce number of files per directory)
   restructure_dir(base_dir);

   file_list = NULL;
   n = 0;
   scan_dir_tree(lbs, base_dir, &file_list, &n);

   /* go through all files */
   for (index = 0; index < n; index++) {
      status = parse_file(lbs, file_list + index * MAX_PATH_LENGTH);
      if (status != SUCCESS) {
         if (file_list)
            xfree(file_list);
         return status;
      }
   }

   if (file_list)
      xfree(file_list);

   /* sort entries according to date */
   qsort(lbs->el_index, *lbs->n_el_index, sizeof(EL_INDEX), eli_compare);

   if (get_verbose() >= VERBOSE_DEBUG) {
      eprintf("After sort:\n");
      for (i = 0; i < *lbs->n_el_index; i++)
         eprintf("  ID %3d, %s, ofs %5d\n", lbs->el_index[i].message_id, lbs->el_index[i].file_name,
                 lbs->el_index[i].offset);
   }

   if (rebuild && n == 0) {
      eprintf("Logbook files seem to have disappeared, aborting program.\n");
      assert(rebuild && n > 0);
   }

   return EL_SUCCESS;
}
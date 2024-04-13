int parse_file(LOGBOOK *lbs, char *file_name) {
   char str[256], date[256], *buffer, *p, *pn, in_reply_to[80];
   int length, i, fh, len;

   fh = open(file_name, O_RDONLY | O_BINARY, 0644);

   if (fh < 0) {
      sprintf(str, "Cannot open file \"%s\"", file_name);
      eprintf("%s; %s\n", str, strerror(errno));
      return EL_FILE_ERROR;
   }

   /* read file into buffer */
   length = lseek(fh, 0, SEEK_END);

   if (length > 0) {
      buffer = xmalloc(length + 1);
      lseek(fh, 0, SEEK_SET);
      read(fh, buffer, length);
      buffer[length] = 0;
      close(fh);

      /* go through buffer */
      p = buffer;

      do {
         p = strstr(p, "$@MID@$:");

         if (p) {
            lbs->el_index = xrealloc(lbs->el_index, sizeof(EL_INDEX) * (*lbs->n_el_index + 1));
            if (lbs->el_index == NULL) {
               eprintf("Not enough memory to allocate entry index\n");
               return EL_MEM_ERROR;
            }

            strlcpy(lbs->el_index[*lbs->n_el_index].subdir, file_name + strlen(lbs->data_dir), 256);
            if (strrchr(lbs->el_index[*lbs->n_el_index].subdir, DIR_SEPARATOR))
               *(strrchr(lbs->el_index[*lbs->n_el_index].subdir, DIR_SEPARATOR) + 1) = 0;

            if (strrchr(file_name, DIR_SEPARATOR))
               strlcpy(str, strrchr(file_name, DIR_SEPARATOR) + 1, sizeof(str));
            else
               strlcpy(str, file_name, sizeof(str));
            strcpy(lbs->el_index[*lbs->n_el_index].file_name, str);

            el_decode(p, "Date: ", date, sizeof(date));
            el_decode_int(p, "In reply to: ", in_reply_to, sizeof(in_reply_to));

            lbs->el_index[*lbs->n_el_index].file_time = date_to_ltime(date);

            lbs->el_index[*lbs->n_el_index].message_id = atoi(p + 8);
            lbs->el_index[*lbs->n_el_index].offset = p - buffer;
            lbs->el_index[*lbs->n_el_index].in_reply_to = atoi(in_reply_to);

            pn = strstr(p + 8, "$@MID@$:");
            if (pn)
               len = pn - p;
            else
               len = strlen(p);

            MD5_checksum(p, len, lbs->el_index[*lbs->n_el_index].md5_digest);

            if (lbs->el_index[*lbs->n_el_index].message_id > 0) {
               if (get_verbose() == VERBOSE_DEBUG) {
                  eprintf("  ID %3d, %s, ofs %5d, %s, MD5=", lbs->el_index[*lbs->n_el_index].message_id,
                          str, lbs->el_index[*lbs->n_el_index].offset,
                          lbs->el_index[*lbs->n_el_index].in_reply_to ? "reply" : "thead");

                  for (i = 0; i < 16; i++)
                     eprintf("%02X", lbs->el_index[*lbs->n_el_index].md5_digest[i]);
                  eprintf("\n");
               }

               /* valid ID */
               (*lbs->n_el_index)++;
            }

            p += 8;
         }

      } while (p);

      xfree(buffer);
   }

   return SUCCESS;
}
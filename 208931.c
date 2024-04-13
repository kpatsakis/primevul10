int show_md5_page(LOGBOOK *lbs) {
   int i, j;
   char *buffer, error_str[256];
   unsigned char digest[16];

   /* header */
   rsprintf("HTTP/1.1 200 Document follows\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());
   rsprintf("Accept-Ranges: bytes\r\n");
   rsprintf("Connection: close\r\n");
   rsprintf("Content-Type: text/plain;charset=%s\r\n", DEFAULT_HTTP_CHARSET);
   rsprintf("Pragma: no-cache\r\n");
   rsprintf("Cache-control: private, max-age=0, no-cache, no-store\r\n\r\n");

   /* calculate MD5 for logbook section in config file */
   load_config_section(lbs->name, &buffer, error_str);
   if (error_str[0])
      rsprintf("<h2>%s</h2>", error_str);
   else {
      rsprintf("ID: %6d MD5:", 0);

      remove_crlf(buffer);
      MD5_checksum(buffer, strlen(buffer), digest);

      for (i = 0; i < 16; i++)
         rsprintf("%02X", digest[i]);
      rsprintf("\n");
   }
   xfree(buffer);

   /* show MD5's of logbook entries */
   for (i = 0; i < *lbs->n_el_index; i++) {
      rsprintf("ID: %6d MD5:", lbs->el_index[i].message_id);
      for (j = 0; j < 16; j++)
         rsprintf("%02X", lbs->el_index[i].md5_digest[j]);
      rsprintf("\n");
   }

   keep_alive = FALSE;

   return EL_SUCCESS;
}
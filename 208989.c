void show_plain_header(int size, char *file_name) {
   /* header */
   rsprintf("HTTP/1.1 200 Document follows\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());
   rsprintf("Accept-Ranges: bytes\r\n");

   if (keep_alive) {
      rsprintf("Connection: Keep-Alive\r\n");
      rsprintf("Keep-Alive: timeout=60, max=10\r\n");
   }
   rsprintf("Pragma: no-cache\r\n");
   rsprintf("Cache-control: private, max-age=0, no-cache, no-store\r\n");
   rsprintf("Content-Type: text/plain\r\n");
   rsprintf("Content-disposition: attachment; filename=\"%s\"\r\n", file_name);
   if (size)
      rsprintf("Content-Length: %d\r\n", size);
   rsprintf("\r\n");
}
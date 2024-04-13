void show_http_header(LOGBOOK *lbs, BOOL expires, char *cookie) {
   char str[256];

   rsprintf("HTTP/1.1 200 Document follows\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());

   if (getcfg("global", "charset", str, sizeof(str)))
      rsprintf("Content-Type: text/html;charset=%s\r\n", str);
   else
      rsprintf("Content-Type: text/html;charset=%s\r\n", DEFAULT_HTTP_CHARSET);

   if (cookie && cookie[0])
      set_cookie(lbs, cookie, NULL, FALSE, "99999");    /* ten years by default */

   if (keep_alive) {
      rsprintf("Connection: Keep-Alive\r\n");
      rsprintf("Keep-Alive: timeout=60, max=10\r\n");
   }

   if (expires) {
      rsprintf("Pragma: no-cache\r\n");
      rsprintf("Cache-control: private, max-age=0, no-cache, no-store\r\n");
   }

   rsprintf("\r\n");
}
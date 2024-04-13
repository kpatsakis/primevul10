void redirect(LOGBOOK *lbs, char *rel_path) {
   /* redirect */
   rsprintf("HTTP/1.1 302 Found\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());
   if (keep_alive) {
      rsprintf("Connection: Keep-Alive\r\n");
      rsprintf("Keep-Alive: timeout=60, max=10\r\n");
   }

   set_location(lbs, rel_path);
}
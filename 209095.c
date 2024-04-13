void mprint(LOGBOOK *lbs, int mode, char *str) {
   char line[1000];

   if (mode == SYNC_HTML)
      rsprintf("%s\n", str);
   else if (mode == SYNC_CRON) {
      if (_logging_level > 1) {
         sprintf(line, "MIRROR: %s", str);
         write_logfile(lbs, line);
      }
   } else
      eputs(str);
}
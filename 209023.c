void print_syslog(const char *msg) {
   char *p;

   /* strip trailing \r and \n */
   p = xstrdup(msg);
   while (p[strlen(p) - 1] == '\r' || p[strlen(p) - 1] == '\n')
      p[strlen(p) - 1] = 0;

#ifdef OS_UNIX
   syslog(SYSLOG_PRIORITY, "%s", p);
#else
   ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, &p, NULL);
#endif

   xfree(p);
}
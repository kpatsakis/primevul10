void write_logfile(LOGBOOK *lbs, const char *text) {
   char file_name[MAX_PATH_LENGTH];
   char str[MAX_PATH_LENGTH], unm[256];
   int fh;
   time_t now;
   char buf[10000];

   if (lbs == NULL) {
      if (!getcfg("global", "logfile", str, sizeof(str)))
         return;
   } else if (!getcfg(lbs->name, "logfile", str, sizeof(str)))
      return;

   if (str[0] == DIR_SEPARATOR || str[1] == ':')
      strlcpy(file_name, str, sizeof(file_name));
   else {
      strlcpy(file_name, logbook_dir, sizeof(file_name));
      strlcat(file_name, str, sizeof(file_name));
   }

   fh = open(file_name, O_RDWR | O_BINARY | O_CREAT | O_APPEND, 0644);
   if (fh < 0)
      return;

   now = time(0);
   strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M:%S", localtime(&now));
   strcat(buf, " ");

   if (isparam("unm") && rem_host[0]) {
      strlcpy(unm, getparam("unm"), sizeof(unm));
      if (rem_host_ip[0])
         sprintf(buf + strlen(buf), "[%s@%s(%s)] ", unm, rem_host, rem_host_ip);
      else
         sprintf(buf + strlen(buf), "[%s@%s] ", unm, rem_host);
   } else if (rem_host[0]) {
      if (rem_host_ip[0])
         sprintf(buf + strlen(buf), "[%s(%s)] ", rem_host, rem_host_ip);
      else
         sprintf(buf + strlen(buf), "[%s] ", rem_host);
   } else
      sprintf(buf + strlen(buf), "[%s] ", rem_host_ip);

   if (lbs)
      sprintf(buf + strlen(buf), "{%s} ", lbs->name);

   strlcat(buf, text, sizeof(buf) - 1);
#ifdef OS_WINNT
                                                                                                                           if (strlen(buf) > 0 && buf[strlen(buf) - 1] != '\n')
      strlcat(buf, "\r\n", sizeof(buf));
   else if (strlen(buf) > 1 && buf[strlen(buf) - 2] != '\r')
      strlcpy(buf + strlen(buf) - 2, "\r\n", sizeof(buf) - (strlen(buf) - 2));

#else
   if (strlen(buf) > 1 && buf[strlen(buf) - 1] != '\n')
      strlcat(buf, "\n", sizeof(buf));
#endif

   write(fh, buf, strlen(buf));

   close(fh);
}
int check_language() {
   char language[256], file_name[256], *p;
   int fh, length, n;
   struct stat cfg_stat;

   getcfg("global", "Language", language, sizeof(language));

   /* set locale for strftime */
   if (language[0])
      setlocale(LC_ALL, language);
   else
      setlocale(LC_ALL, "english");

   /* force re-read configuration file if changed */
   strlcpy(file_name, resource_dir, sizeof(file_name));
   strlcat(file_name, "resources", sizeof(file_name));
   strlcat(file_name, DIR_SEPARATOR_STR, sizeof(file_name));
   strlcat(file_name, "eloglang.", sizeof(file_name));
   strlcat(file_name, language, sizeof(file_name));

   if (stat(file_name, &cfg_stat) == 0) {
      if (_locfile_mtime != cfg_stat.st_mtime) {
         _locfile_mtime = cfg_stat.st_mtime;

         if (_locbuffer) {
            xfree(_locbuffer);
            _locbuffer = NULL;
         }
      }
   }

   if (strieq(language, "english") || language[0] == 0) {
      if (_locbuffer) {
         xfree(_locbuffer);
         _locbuffer = NULL;
      }
   } else {
      if (_locbuffer == NULL) {
         fh = open(file_name, O_RDONLY | O_BINARY);
         if (fh < 0)
            return -1;

         length = lseek(fh, 0, SEEK_END);
         lseek(fh, 0, SEEK_SET);
         _locbuffer = xmalloc(length + 1);
         read(fh, _locbuffer, length);
         _locbuffer[length] = 0;
         close(fh);

         /* scan lines, setup orig-translated pointers */
         p = _locbuffer;
         n = 0;
         do {
            while (*p && (*p == '\r' || *p == '\n'))
               p++;

            if (*p && (*p == ';' || *p == '#' || *p == ' ' || *p == '\t')) {
               while (*p && *p != '\n' && *p != '\r')
                  p++;
               continue;
            }

            if (n == 0) {
               _porig = xmalloc(sizeof(char *) * 2);
               _ptrans = xmalloc(sizeof(char *) * 2);
            } else {
               _porig = xrealloc(_porig, sizeof(char *) * (n + 2));
               _ptrans = xrealloc(_ptrans, sizeof(char *) * (n + 2));
            }

            _porig[n] = p;
            while (*p && (*p != '=' && *p != '\r' && *p != '\n'))
               p++;

            if (*p && *p != '=')
               continue;

            _ptrans[n] = p + 1;
            while (*_ptrans[n] == ' ' || *_ptrans[n] == '\t')
               _ptrans[n]++;

            /* remove '=' and surrounding blanks */
            while (*p == '=' || *p == ' ' || *p == '\t')
               *p-- = 0;

            p = _ptrans[n];
            while (*p && *p != '\n' && *p != '\r')
               p++;

            if (p)
               *p++ = 0;

            n++;
         } while (p && *p);

         _porig[n] = NULL;
         _ptrans[n] = NULL;
      }
   }

   return 0;
}
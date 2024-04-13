int scan_dir_tree(LOGBOOK *lbs, const char *dir, char **file_list, int *n) {
   int index, n_files;
   char str[MAX_PATH_LENGTH];
   char *fl, *p;

   fl = NULL;
   n_files = ss_file_find(dir, "*", &fl);
   if (n_files == 0) {
      if (fl)
         xfree(fl);
      return 0;
   }

   if (*file_list == NULL)
      *file_list = (char *) xmalloc(n_files * MAX_PATH_LENGTH);
   else
      *file_list = (char *) xrealloc(*file_list, ((*n) + n_files) * MAX_PATH_LENGTH);

   /* go through all files */
   for (index = 0; index < n_files; index++) {
      if (fnmatch1("??????a.log", &fl[index * MAX_PATH_LENGTH]) == 0) {
         p = *file_list + ((*n) * MAX_PATH_LENGTH);
         strlcpy(p, dir, MAX_PATH_LENGTH);
         if (p[strlen(p) - 1] != DIR_SEPARATOR)
            strlcat(p, DIR_SEPARATOR_STR, MAX_PATH_LENGTH);
         strlcat(p, fl + index * MAX_PATH_LENGTH, MAX_PATH_LENGTH);
         (*n)++;
      }
   }

   /* go through all sub-directories */
   for (index = 0; index < n_files; index++) {
      if (fnmatch1("????", &fl[index * MAX_PATH_LENGTH]) == 0 ||
          fnmatch1("??", &fl[index * MAX_PATH_LENGTH]) == 0) {
         if (strieq(fl + index * MAX_PATH_LENGTH, ".."))
            continue;
         strlcpy(str, dir, sizeof(str));
         if (str[strlen(str) - 1] != DIR_SEPARATOR)
            strlcat(str, DIR_SEPARATOR_STR, sizeof(str));
         strlcat(str, fl + index * MAX_PATH_LENGTH, sizeof(str));
         scan_dir_tree(lbs, str, file_list, n);
      }
   }

   if (fl)
      xfree(fl);

   return *n;
}
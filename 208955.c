int create_thumbnail(LOGBOOK *lbs, char *file_name) {
   char str[2 * MAX_PATH_LENGTH], cmd[2 * MAX_PATH_LENGTH], thumb_size[256], thumb_options[256];
   int i;

   if (!image_magick_exist)
      return 0;

   if (getcfg(lbs->name, "Thumbnail size", str, sizeof(str))) {
      if (strcmp(str, "0") == 0)
         return 0;
      sprintf(thumb_size, " -thumbnail '%s'", str);
   } else
      thumb_size[0] = 0;

   getcfg(lbs->name, "Thumbnail options", thumb_options, sizeof(thumb_options));

   if (!chkext(file_name, ".ps") && !chkext(file_name, ".pdf") && !chkext(file_name, ".eps")
       && !chkext(file_name, ".gif") && !chkext(file_name, ".jpg") && !chkext(file_name, ".jpeg")
       && !chkext(file_name, ".png") && !chkext(file_name, ".ico") && !chkext(file_name, ".tif")
       && !chkext(file_name, ".svg"))
      return 0;

   i = get_thumb_name(file_name, str, sizeof(str), 0);
   if (i)
      return i;

   strlcpy(str, file_name, sizeof(str));
   if (chkext(file_name, ".pdf") || chkext(file_name, ".ps")) {
      if (strrchr(str, '.'))
         *strrchr(str, '.') = 0;
   }
   if (chkext(file_name, ".pdf") || chkext(file_name, ".ps"))
      strlcat(str, "-%d.png", sizeof(str));
   else
      strlcat(str, ".png", sizeof(str));

   if (chkext(file_name, ".pdf") || chkext(file_name, ".ps"))
      snprintf(cmd, sizeof(cmd), "%s %s '%s[0-7]'%s '%s'", _convert_cmd, thumb_options, file_name, thumb_size,
               str);
   else
      snprintf(cmd, sizeof(cmd), "%s %s '%s'%s '%s'", _convert_cmd, thumb_options, file_name, thumb_size, str);

#ifdef OS_WINNT
                                                                                                                           for (i = 0; i < (int) strlen(cmd); i++)
      if (cmd[i] == '\'')
         cmd[i] = '\"';
#endif

   snprintf(str, sizeof(str), "SHELL \"%s\"", cmd);
   write_logfile(lbs, str);
   if (get_verbose() >= VERBOSE_INFO) {
      eprintf(str);
      eprintf("\n");
   }

   my_shell(cmd, str, sizeof(str));

   i = get_thumb_name(file_name, str, sizeof(str), 0);
   if (i)
      return i;

   return 3;
}
BOOL get_password_file(LOGBOOK *lbs, char *file_name, int size) {
   char str[256];

   getcfg(lbs->name, "Password file", str, sizeof(str));

   if (!str[0])
      return FALSE;

   if (str[0] == DIR_SEPARATOR || str[1] == ':')
      strlcpy(file_name, str, size);
   else {
      strlcpy(file_name, logbook_dir, size);
      strlcat(file_name, str, size);
   }

   return TRUE;
}
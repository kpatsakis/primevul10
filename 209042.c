void show_top_text(LOGBOOK *lbs) {
   char str[NAME_LENGTH];
   int size;

   if (getcfg(lbs->name, "top text", str, sizeof(str)) && str[0]) {
      FILE *f;
      char file_name[256], *buf;

      /* check if file starts with an absolute directory */
      if (str[0] == DIR_SEPARATOR || str[1] == ':')
         strcpy(file_name, str);
      else {
         strlcpy(file_name, logbook_dir, sizeof(file_name));
         strlcat(file_name, str, sizeof(file_name));
      }

      f = fopen(file_name, "rb");
      if (f != NULL) {
         fseek(f, 0, SEEK_END);
         size = TELL(fileno(f));
         fseek(f, 0, SEEK_SET);

         buf = xmalloc(size + 1);
         fread(buf, 1, size, f);
         buf[size] = 0;
         fclose(f);

         rsputs(buf);
      } else
         rsputs(str);
   }
}
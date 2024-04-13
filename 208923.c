void show_bottom_text_login(LOGBOOK *lbs) {
   char str[NAME_LENGTH], slist[20][NAME_LENGTH], svalue[20][NAME_LENGTH];
   int i, size;

   if (getcfg(lbs->name, "bottom text login", str, sizeof(str))) {
      FILE *f;
      char file_name[256], *buf;

      if (str[0]) {
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

            buf = xmalloc(size + 100);
            fread(buf, 1, size, f);
            buf[size] = 0;
            fclose(f);

            if (lbs != NULL) {
               i = build_subst_list(lbs, slist, svalue, NULL, TRUE);
               strsubst_list(buf, size + 100, slist, svalue, i);
            }

            rsputs(buf);
            xfree(buf);
         } else {
            if (lbs != NULL) {
               i = build_subst_list(lbs, slist, svalue, NULL, TRUE);
               strsubst_list(str, sizeof(str), slist, svalue, i);
            }

            rsputs(str);
         }
      }
   } else
      /* add little logo */
      rsprintf
              ("<center><a class=\"bottomlink\" title=\"%s\" href=\"https://elog.psi.ch/elog/\">ELOG V%s-%s</a></center>",
               loc("Goto ELOG home page"), VERSION, git_revision());
}
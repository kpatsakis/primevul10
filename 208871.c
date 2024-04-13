int execute_shell(LOGBOOK *lbs, int message_id, char attrib[MAX_N_ATTR][NAME_LENGTH],
                  char att_file[MAX_ATTACHMENTS][256], char *sh_cmd) {
   int i;
   char slist[MAX_N_ATTR + 10][NAME_LENGTH], svalue[MAX_N_ATTR + 10][NAME_LENGTH];
   char shell_cmd[10000], tail[1000], str[NAME_LENGTH], *p, subdir[256];

   if (!enable_execute) {
      eprintf("Shell execution not enabled via -x flag.\n");
      return SUCCESS;
   }

   strlcpy(shell_cmd, sh_cmd, sizeof(shell_cmd));

   i = build_subst_list(lbs, slist, svalue, attrib, TRUE);
   sprintf(str, "%d", message_id);
   add_subst_list(slist, svalue, "message id", str, &i);
   add_subst_list(slist, svalue, "text", getparam("text"), &i);
   strsubst_list(shell_cmd, sizeof(shell_cmd), slist, svalue, i);

   if (att_file && stristr(shell_cmd, "$attachments")) {
      /* substitute attachments */
      p = stristr(shell_cmd, "$attachments");
      strlcpy(tail, p + strlen("$attachments"), sizeof(tail));
      *p = 0;
      for (i = 0; i < MAX_ATTACHMENTS; i++) {
         generate_subdir_name(att_file[i], subdir, sizeof(subdir));
         if (att_file[i][0] && strlen(shell_cmd) + strlen(lbs->data_dir) + strlen(subdir) + strlen(att_file[i])
                               < sizeof(shell_cmd) + 1) {
            strcpy(p, "\"");
            strcat(p, lbs->data_dir);
            strlcat(str, subdir, sizeof(str));
            strlcpy(str, att_file[i], sizeof(str));
            str_escape(str, sizeof(str));
            strcat(p, str);
            strcat(p, "\" ");
            p += strlen(p);
         }
      }
      strlcat(shell_cmd, tail, sizeof(shell_cmd));
   }

   sprintf(str, "SHELL \"%s\"", shell_cmd);
   write_logfile(lbs, str);

   my_shell(shell_cmd, str, sizeof(str));

   return SUCCESS;
}
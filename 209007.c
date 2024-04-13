int setparam(char *param, char *value) {
   int i;
   char str[10000];

   if (strieq(param, "text")) {
      if (strlen(value) >= TEXT_SIZE) {
         sprintf(str,
                 "Error: Entry text too big (%lu bytes). Please increase TEXT_SIZE and recompile elogd\n",
                 (unsigned long) strlen(value));
         show_error(str);
         return 0;
      }

      strlcpy(_mtext, value, TEXT_SIZE);
      return 1;
   }

   if (strieq(param, "cmdline")) {
      if (strlen(value) >= CMD_SIZE) {
         sprintf(str,
                 "Error: Command line too big (%lu bytes). Please increase CMD_SIZE and recompile elogd\n",
                 (unsigned long) strlen(value));
         show_error(str);
         return 0;
      }

      strlcpy(_cmdline, value, CMD_SIZE);
      return 1;
   }

   /* paremeters can be superseeded */
   for (i = 0; i < MAX_PARAM; i++)
      if (_param[i][0] == 0 || strieq(param, _param[i]))
         break;

   if (i < MAX_PARAM) {
      if (strlen(param) >= NAME_LENGTH) {
         sprintf(str, "Error: Parameter name too big (%lu bytes).\n", (unsigned long) strlen(param));
         show_error(str);
         return 0;
      }

      strlcpy(_param[i], param, NAME_LENGTH);

      if (strlen(value) >= NAME_LENGTH) {
         sprintf(str,
                 "Error: Parameter value for parameter <b>%s</b> too big (%lu bytes). Please increase NAME_LENGTH and recompile elogd\n",
                 param, (unsigned long) strlen(value));
         show_error(str);
         return 0;
      }

      strlcpy(_value[i], value, NAME_LENGTH);
   } else {
      sprintf(str, "Error: Too many parameters (> %d). Cannot perform operation.\n", MAX_PARAM);
      show_error(str);
      return 0;
   }

   return 1;
}
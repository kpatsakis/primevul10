void el_enum_attr(char *message, int n, char *attr_name, char *attr_value) {
   char *p, str[NAME_LENGTH], tmp[NAME_LENGTH];
   int i;

   p = message;
   for (i = 0; i <= n; i++) {
      strlcpy(str, p, sizeof(str));
      if (strchr(str, '\n'))
         *strchr(str, '\n') = 0;
      if (strchr(str, '\r'))
         *strchr(str, '\r') = 0;

      if (strcmp(str, "========================================") == 0)
         break;

      p = strchr(p, '\n');
      if (!p) {
         str[0] = 0;            /* not a valid line */
         break;
      }
      while (*p == '\n' || *p == '\r')
         p++;

      if (strchr(str, ':')) {
         strcpy(tmp, str);
         *strchr(tmp, ':') = 0;

         if (strieq(tmp, "$@MID@$") || strieq(tmp, "Date") || strieq(tmp, "Attachment") || strieq(tmp,
                                                                                                  "Reply To")
             || strieq(tmp, "In Reply To") || strieq(tmp, "Encoding") || strieq(tmp, "Locked by"))
            i--;
      }
   }

   attr_name[0] = 0;
   attr_value[0] = 0;
   if (strchr(str, ':')) {
      strlcpy(attr_name, str, NAME_LENGTH);
      *strchr(attr_name, ':') = 0;
      strlcpy(attr_value, strchr(str, ':') + 2, NAME_LENGTH);
   }

}
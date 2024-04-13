BOOL subst_param(char *str, int size, char *param, char *value) {
   int len;
   char *p1, *p2, *s, param_enc[256], str2[256];

   strlcpy(param_enc, param, sizeof(param_enc));
   url_slash_encode(param_enc, sizeof(param_enc));

   if (!value[0]) {

      /* remove parameter */
      s = param_in_str(str, param_enc);

      if (s == NULL)
         return FALSE;

      /* remove parameter */
      p1 = s - 1;

      for (p2 = p1 + strlen(param_enc) + 1; *p2 && *p2 != '&'; p2++);
      strlcpy(str2, p2, sizeof(str2));
      strlcpy(p1, str2, size - (p1 - str));

      if (!strchr(str, '?') && strchr(str, '&'))
         *strchr(str, '&') = '?';

      return TRUE;
   }

   if ((p1 = param_in_str(str, param_enc)) == NULL) {
      if (strchr(str, '?'))
         strlcat(str, "&", size);
      else
         strlcat(str, "?", size);

      strlcat(str, param_enc, size);
      strlcat(str, "=", size);
      strlcat(str, value, size);
      return FALSE;
   }

   p1 += strlen(param_enc) + 1;
   for (p2 = p1; *p2 && *p2 != '&'; p2++);
   len = p2 - p1;
   if (len > (int) strlen(value)) {
      /* new value is shorter than old one */
      strlcpy(str2, value, size - (p1 - str));
      strlcpy(str2 + strlen(value), p2, size - (p1 + strlen(value) - str));
      strlcpy(p1, str2, size - (p1 - str));
   } else {
      /* new value is longer than old one */
      s = (char *) xmalloc(size);
      strlcpy(s, p2, size);
      strlcpy(str2, value, size - (p1 - str));
      strlcat(str2, s, size - (p1 + strlen(value) - str));
      strlcpy(p1, str2, size - (p1 - str));
      xfree(s);
   }

   return TRUE;
}
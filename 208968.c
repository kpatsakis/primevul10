void build_ref(char *ref, int size, char *mode, char *expand, char *attach, char *new_entries) {
   char str[1000], *p;

   if (strchr(getparam("cmdline"), '?'))
      strlcat(ref, strchr(getparam("cmdline"), '?'), size);

   /* eliminate old search */
   if (strstr(ref, "cmd=Search&")) {
      strlcpy(str, strstr(ref, "cmd=Search&") + 11, sizeof(str));
      p = strstr(ref, "cmd=Search&");
      strlcpy(p, str, size - (p - ref));
   }

   /* eliminate id=xxx */
   if (strstr(ref, "id=")) {
      strlcpy(str, ref, sizeof(str));
      p = strstr(str, "id=") + 3;
      while (*p && isdigit(*p))
         p++;
      strlcpy(strstr(ref, "id="), p, size);
      if (strlen(ref) > 0 && ref[strlen(ref) - 1] == '?')
         ref[strlen(ref) - 1] = 0;
   }

   /* eliminate old mode if new one is present */
   if (mode[0])
      subst_param(ref, size, "mode", mode);

   /* eliminate old expand if new one is present */
   if (expand[0])
      subst_param(ref, size, "expand", expand);

   /* eliminate old attach if new one is present */
   if (attach[0])
      subst_param(ref, size, "attach", attach);

   /* eliminate old new_entries if new one is present */
   if (new_entries[0])
      subst_param(ref, size, "new_entries", new_entries);

   /* eliminate old last= */
   if (isparam("last"))
      subst_param(ref, size, "last", getparam("last"));

   /* replace any '&' by '&amp;' */
   strlcpy(str, ref, sizeof(str));
   strencode2(ref, str, size);
}
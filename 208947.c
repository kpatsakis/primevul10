int is_inline_attachment(char *encoding, int message_id, char *text, int i, char *att) {
   char str[256], att_enc[256], domain[256], *pt, *p;

   if (text == NULL)
      return 0;
   if (strieq(encoding, "ELCode")) {
      sprintf(str, "[img]elog:/%d[/img]", i + 1);
      if (stristr(text, str))
         return 1;
      sprintf(str, "[img]elog:%d/%d[/img]", message_id, i + 1);
      if (stristr(text, str))
         return 1;
   } else if (strieq(encoding, "HTML")) {
      strlcpy(att_enc, att, sizeof(att_enc));
      att_enc[13] = '/';
      pt = text;
      while (*pt && stristr(pt, att_enc)) {
         /* make sure that it's really an inline image */
         for (p = stristr(pt, att_enc); p > pt; p--)
            if (*p == '<')
               break;
         if (p > pt) {
            strncpy(str, p, 5);
            if (stristr(str, "<img "))
               return 1;
         }
         pt = stristr(pt, att_enc) + 1;
      }
      retrieve_domain(domain, sizeof(domain));
      sprintf(str, "cid:att%d@%s", i, domain);
      if (*text && stristr(text, str))
         return 1;
   }

   return 0;
}
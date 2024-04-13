void replace_inline_img(LOGBOOK *lbs, char *str) {
   char *p, *pn, *pa, old[256], link[256], base_url[256], domain[256];
   int index;

   p = str;
   do {
      p = strstr(p, "<img ");
      if (p) {
         pn = strstr(p, "name=");
         if (pn) {
            pn += 9;
            index = atoi(pn);
            while (*pn && *pn != '>')
               pn++;
            if (*pn == '>')
               pn++;
            retrieve_domain(domain, sizeof(domain));
            sprintf(p, "<img border=\"0\" src=\"cid:att%d@%s\">", index, domain);
            memmove(p + strlen(p), pn, strlen(pn) + 1);

            /* now change href to absolute link */
            pa = p - 1;
            while (pa > str && *pa != '<')
               // search '<a href=...>'
               pa--;
            pn = strstr(pa, "href=");
            if (pn && pn - pa < 10) {
               strlcpy(old, pn + 6, sizeof(old));
               if (strchr(old, '\"'))
                  *strchr(old, '\"') = 0;
               compose_base_url(lbs, base_url, sizeof(base_url), FALSE);
               strlcpy(link, base_url, sizeof(link));
               strlcat(link, old, sizeof(link));
               if (strchr(link, '?'))
                  *strchr(link, '?') = 0;
               strsubst(pn + 6, TEXT_SIZE, old, link);
               if (strlen(link) > strlen(old))
                  p += strlen(link) - strlen(old);
            }

            p++;
         } else
            p++;
      }
   } while (p != NULL);
}
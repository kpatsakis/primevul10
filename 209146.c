void set_location(LOGBOOK *lbs, char *rp) {
   char str[NAME_LENGTH], group[NAME_LENGTH], list[NAME_LENGTH], *p, rel_path[NAME_LENGTH];
   int i;

   /* remove any CR/LF from path */
   strlcpy(rel_path, rp, sizeof(rel_path));
   if (strchr(rel_path, '\r'))
      *strchr(rel_path, '\r') = 0;
   if (strchr(rel_path, '\n'))
      *strchr(rel_path, '\n') = 0;

   if (getcfg(lbs->name, "Relative redirect", str, sizeof(str)) && atoi(str) == 1) {

      if (rel_path[0])
         strlcpy(str, rel_path, sizeof(str));
      else
         strlcpy(str, ".", sizeof(str));

      rsputs("Location: ");
      rsputs(str);

   } else {
      /* Absolute redirect */

      /* if path contains http(s), go directly there */
      if (strncmp(rel_path, "http://", 7) == 0) {
         rsputs("Location: ");
         rsputs(rel_path);
      } else if (strncmp(rel_path, "https://", 8) == 0) {
         rsputs("Location: ");
         rsputs(rel_path);
      } else {

         /* check for URL options */
         str[0] = 0;
         if (lbs)
            getcfg(lbs->name, "URL", str, sizeof(str));
         else
            getcfg("global", "URL", str, sizeof(str));

         if (str[0] == 0) {

            /* get redirection from referer and host */

            if (referer[0]) {
               strlcpy(str, referer, sizeof(str));

               /* strip any parameter */
               if (strchr(str, '?'))
                  *strchr(str, '?') = 0;

               /* strip rightmost '/' */
               if (str[strlen(str) - 1] == '/')
                  str[strlen(str) - 1] = 0;

               /* extract last subdir */
               p = str + strlen(str);
               while (p > str && *p != '/')
                  p--;
               if (*p == '/')
                  p++;

               /* if last subdir equals any logbook name, strip it */
               for (i = 0; lb_list[i].name[0]; i++)
                  if (stricmp(p, lb_list[i].name_enc) == 0) {
                     *p = 0;
                     break;
                  }

               /* if last subdir equals any group, strip it */
               snprintf(group, sizeof(group), "Group %s", p);
               if (getcfg("global", group, list, sizeof(list)))
                  *p = 0;

               /* if last subdir equals any top group, strip it */
               snprintf(group, sizeof(group), "Top group %s", p);
               if (getcfg("global", group, list, sizeof(list)))
                  *p = 0;

            } else {
               /* assemble absolute path from host name and port */
               if (_ssl_flag)
                  snprintf(str, sizeof(str), "https://%s", http_host);
               else
                  snprintf(str, sizeof(str), "http://%s", http_host);
               if (elog_tcp_port != 80 && strchr(str, ':') == NULL)
                  sprintf(str + strlen(str), ":%d", elog_tcp_port);
               strlcat(str, "/", sizeof(str));
            }

            /* add trailing '/' if not present */
            if (str[strlen(str) - 1] != '/')
               strlcat(str, "/", sizeof(str));

            /* add top group if existing and not logbook */
            if (!lbs && getcfg_topgroup()) {
               strlcat(str, getcfg_topgroup(), sizeof(str));
               strlcat(str, "/", sizeof(str));
            }

            if (strncmp(rel_path, "../", 3) == 0)
               strlcat(str, rel_path + 3, sizeof(str));
            else if (strcmp(rel_path, ".") == 0) {
               if (lbs)
                  strlcat(str, lbs->name_enc, sizeof(str));
            } else if (rel_path[0] == '/')
               strlcat(str, rel_path + 1, sizeof(str));
            else {
               if (lbs) {
                  strlcat(str, lbs->name_enc, sizeof(str));
                  strlcat(str, "/", sizeof(str));
                  strlcat(str, rel_path, sizeof(str));
               } else
                  strlcat(str, rel_path, sizeof(str));
            }

            rsputs("Location: ");
            rsputs(str);

         } else {

            /* use redirection via URL */

            /* if HTTP request comes from localhost, use localhost as
               absolute link (needed if running on DSL at home) */
            if (!str[0] && strstr(http_host, "localhost")) {
               if (_ssl_flag)
                  strlcpy(str, "https://localhost", sizeof(str));
               else
                  strlcpy(str, "http://localhost", sizeof(str));
               if (elog_tcp_port != 80)
                  sprintf(str + strlen(str), ":%d", elog_tcp_port);
               strlcat(str, "/", sizeof(str));
            }

            /* add trailing '/' if not present */
            if (str[strlen(str) - 1] != '/')
               strlcat(str, "/", sizeof(str));

            /* add top group if existing and not logbook */
            if (!lbs && getcfg_topgroup()) {
               strlcat(str, getcfg_topgroup(), sizeof(str));
               strlcat(str, "/", sizeof(str));
            }

            if (strncmp(rel_path, "../", 3) == 0)
               strlcat(str, rel_path + 3, sizeof(str));
            else if (strcmp(rel_path, ".") == 0) {
               if (lbs)
                  strlcat(str, lbs->name_enc, sizeof(str));
            } else if (rel_path[0] == '/')
               strlcat(str, rel_path + 1, sizeof(str));
            else {
               if (lbs) {
                  strlcat(str, lbs->name_enc, sizeof(str));
                  strlcat(str, "/", sizeof(str));
                  strlcat(str, rel_path, sizeof(str));
               } else
                  strlcat(str, rel_path, sizeof(str));
            }

            rsputs("Location: ");
            rsputs(str);
         }
      }
   }

   rsprintf("\r\n\r\n<html>redir</html>\r\n");
}
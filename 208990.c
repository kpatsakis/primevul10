void rsputs2(LOGBOOK *lbs, int absolute_link, const char *str) {
   int i, j, k, l, n;
   char *p, *pd, link[1000], link_text[1000];

   if (strlen_retbuf + (int) (2 * strlen(str) + 1000) >= return_buffer_size) {
      return_buffer = xrealloc(return_buffer, return_buffer_size + 100000);
      memset(return_buffer + return_buffer_size, 0, 100000);
      return_buffer_size += 100000;
   }

   j = strlen_retbuf;
   for (i = 0; i < (int) strlen(str); i++) {
      for (l = 0; key_list[l][0]; l++) {
         if (strncmp(str + i, key_list[l], strlen(key_list[l])) == 0) {

            /* check for escape character */
            if (i > 0 && *(str + i - 1) == '\\') {
               j--;
               *(return_buffer + j) = 0;
               continue;
            }

            p = (char *) (str + i + strlen(key_list[l]));
            i += strlen(key_list[l]);
            for (k = 0; *p && strcspn(p, " \t\n\r({[)}]\"") && k < (int) sizeof(link); k++, i++)
               link[k] = *p++;
            link[k] = 0;
            i--;

            /* link may not end with a '.'/',' (like in a sentence) */
            if (link[k - 1] == '.' || link[k - 1] == ',') {
               link[k - 1] = 0;
               k--;
               i--;
            }

            /* check if link contains coloring */
            p = strchr(link, '\001');
            if (p != NULL) {
               strlcpy(link_text, link, sizeof(link_text));

               /* skip everything between '<' and '>' */
               pd = p;
               while (*pd && *pd != '\002')
                  *p = *pd++;

               memmove(p, pd + 1, strlen(pd + 1) + 1);

               /* skip '</B>' */
               p = strchr(link, '\001');
               if (p != NULL) {
                  pd = p;

                  while (*pd && *pd != '\002')
                     *p = *pd++;

                  memmove(p, pd + 1, strlen(pd + 1) + 1);
               }

               /* correct link text */
               for (n = 0; n < (int) strlen(link_text); n++) {
                  switch (link_text[n]) {
                     /* the translation for the search highliting */
                     case '\001':
                        link_text[n] = '<';
                        break;
                     case '\002':
                        link_text[n] = '>';
                        break;
                     case '\003':
                        link_text[n] = '\"';
                        break;
                     case '\004':
                        link_text[n] = ' ';
                        break;
                  }
               }

            } else
               strlcpy(link_text, link, sizeof(link_text));

            if (strcmp(key_list[l], "elog:") == 0) {
               convert_elog_link(lbs, link, link_text, return_buffer + j, absolute_link, _current_message_id);
            } else if (strcmp(key_list[l], "mailto:") == 0) {
               sprintf(return_buffer + j, "<a href=\"mailto:%s\">%s</a>", link, link_text);
            } else {
               sprintf(return_buffer + j, "<a href=\"%s", key_list[l]);
               j += strlen(return_buffer + j);
               strlen_retbuf = j;

               /* link can contain special characters */
               rsputs2(lbs, absolute_link, link);
               j = strlen_retbuf;

               sprintf(return_buffer + j, "\">%s", key_list[l]);
               j += strlen(return_buffer + j);
               strlen_retbuf = j;

               /* link_text can contain special characters */
               rsputs2(lbs, absolute_link, link_text);
               j = strlen_retbuf;
               sprintf(return_buffer + j, "</a>");
            }

            j += strlen(return_buffer + j);
            break;
         }
      }

      if (!key_list[l][0]) {
         if (strncmp(str + i, "<br>", 4) == 0) {
            strcpy(return_buffer + j, "<br>");
            j += 4;
            i += 3;
         } else
            switch (str[i]) {
               case '&':
                  strcat(return_buffer, "&amp;");
                  j += 5;
                  break;
               case '<':
                  strcat(return_buffer, "&lt;");
                  j += 4;
                  break;
               case '>':
                  strcat(return_buffer, "&gt;");
                  j += 4;
                  break;

                  /* suppress escape character '\' in front of HTML or ELCode tag */
               case '\\':
                  if (str[i + 1] != '<' && str[i + 1] != '[')
                     return_buffer[j++] = str[i];
                  break;

                  /* the translation for the search highliting */
               case '\001':
                  strcat(return_buffer, "<");
                  j++;
                  break;
               case '\002':
                  strcat(return_buffer, ">");
                  j++;
                  break;
               case '\003':
                  strcat(return_buffer, "\"");
                  j++;
                  break;
               case '\004':
                  strcat(return_buffer, " ");
                  j++;
                  break;

               default:
                  return_buffer[j++] = str[i];
            }
      }
   }

   return_buffer[j] = 0;
   strlen_retbuf = j;
}
void rsputs_elcode(LOGBOOK *lbs, BOOL email_notify, const char *str) {
   int i, j, k, l, m, elcode_disabled, elcode_disabled1, ordered_list, substituted, inside_table,
           smileys_enabled;
   char *p, *pd, link[1000], link_text[1000], tmp[1000], attrib[1000], hattrib[1000], value[1000],
           subst[1000], base_url[256], param[256], *lstr, domain[256];

   if (strlen_retbuf + (int) (2 * strlen(str) + 1000) >= return_buffer_size) {
      return_buffer = xrealloc(return_buffer, return_buffer_size + 100000);
      memset(return_buffer + return_buffer_size, 0, 100000);
      return_buffer_size += 100000;
   }

   elcode_disabled = FALSE;
   elcode_disabled1 = FALSE;
   ordered_list = FALSE;
   smileys_enabled = TRUE;
   inside_table = 0;
   j = strlen_retbuf;
   m = 0;

   /* check for smileys */
   if (getcfg(lbs->name, "Enable smileys", tmp, sizeof(tmp)) && atoi(tmp) == 0)
      smileys_enabled = FALSE;

   /* make lower case copy of str */
   lstr = xmalloc(strlen(str) + 1);
   for (pd = lstr, p = (char *) str; *p; p++, pd++)
      *pd = tolower(*p);
   *pd = 0;

   for (i = 0; i < (int) strlen(str); i++) {

      for (l = 0; key_list[l][0]; l++) {
         if (strncmp(lstr + i, key_list[l], strlen(key_list[l])) == 0) {

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

            strlcpy(link_text, link, sizeof(link_text));

            /* check if link contains coloring */
            while ((p = strchr(link, '\001')) != NULL) {

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
            }

            if (strcmp(key_list[l], "elog:") == 0) {
               strlcpy(tmp, link, sizeof(tmp));
               if (strchr(tmp, '/'))
                  *strchr(tmp, '/') = 0;

               for (m = 0; m < (int) strlen(tmp); m++)
                  if (!isdigit(tmp[m]))
                     break;

               if (m < (int) strlen(tmp) && tmp[m] != '#') {
                  /* if link contains reference to other logbook, put logbook explicitly */
                  if (email_notify)
                     compose_base_url(NULL, base_url, sizeof(base_url), TRUE);
                  else
                     strcpy(base_url, "../");
                  sprintf(return_buffer + j, "<a href=\"%s%s\">elog:%s</a>", base_url, link, link_text);
               } else if (link[0] == '/') {
                  if (email_notify)
                     compose_base_url(NULL, base_url, sizeof(base_url), TRUE);
                  else
                     base_url[0] = 0;
                  sprintf(return_buffer + j, "<a href=\"%s%s/%d%s\">elog:%s</a>", base_url, lbs->name_enc,
                          _current_message_id, link, link_text);
               } else {
                  if (email_notify)
                     compose_base_url(lbs, base_url, sizeof(base_url), TRUE);
                  else
                     base_url[0] = 0;
                  sprintf(return_buffer + j, "<a href=\"%s%s\">elog:%s</a>", base_url, link, link_text);
               }
            } else if (strcmp(key_list[l], "mailto:") == 0) {
               sprintf(return_buffer + j, "<a href=\"mailto:%s\">%s</a>", link, link_text);
            } else {
               sprintf(return_buffer + j, "<a href=\"%s", key_list[l]);
               j += strlen(return_buffer + j);
               strlen_retbuf = j;

               /* link can contain special characters */
               rsputs2(lbs, email_notify, link);
               j = strlen_retbuf;

               sprintf(return_buffer + j, "\">%s", key_list[l]);
               j += strlen(return_buffer + j);
               strlen_retbuf = j;

               /* link_text can contain special characters */
               rsputs2(lbs, email_notify, link_text);
               j = strlen_retbuf;
               sprintf(return_buffer + j, "</a>");
            }

            j += strlen(return_buffer + j);
            break;
         }
      }
      if (key_list[l][0])
         continue;

      substituted = FALSE;

      for (l = 0; pattern_list[l].pattern[0]; l++) {
         if (strncmp(lstr + i, pattern_list[l].pattern, strlen(pattern_list[l].pattern)) == 0) {

            if (stristr(pattern_list[l].pattern, "[/code]"))
               elcode_disabled = FALSE;
            if (stristr(pattern_list[l].pattern, "[/code1]"))
               elcode_disabled1 = FALSE;

            /* check for escape character */
            if (i > 0 && str[i - 1] == '\\') {

               j--;
               strncpy(return_buffer + j, str + i, strlen(pattern_list[l].pattern));
               j += strlen(pattern_list[l].pattern);
               i += strlen(pattern_list[l].pattern) - 1;        // 1 gets added in for loop...
               substituted = TRUE;

               break;
            }

            /* check for blank before smiley and if smileys are allowed */
            if (l <= 20 &&
                ((str[i - 1] != ' ' && str[i - 1] != '\r' && str[i - 1] != '\n') ||
                 (smileys_enabled == FALSE))) {
               strncpy(return_buffer + j, str + i, strlen(pattern_list[l].pattern));
               j += strlen(pattern_list[l].pattern);
               i += strlen(pattern_list[l].pattern) - 1;        // 1 gets added in for loop...
               substituted = TRUE;

               break;
            }

            if (!elcode_disabled && !elcode_disabled1) {

               substituted = TRUE;

               if (stristr(pattern_list[l].pattern, "[list="))
                  ordered_list = TRUE;

               if (stristr(pattern_list[l].pattern, "[table"))
                  inside_table++;
               if (stristr(pattern_list[l].pattern, "[/table]"))
                  inside_table--;

               if (stristr(pattern_list[l].pattern, "[quote")) {
                  if (pattern_list[l].pattern[strlen(pattern_list[l].pattern) - 1] == '=') {
                     i += strlen(pattern_list[l].pattern);
                     strextract(str + i, ']', attrib, sizeof(attrib));
                     i += strlen(attrib);

                     if (attrib[0] == '\"')
                        memmove(attrib, attrib + 1, strlen(attrib + 1) + 1);
                     if (attrib[strlen(attrib) - 1] == '\"')
                        attrib[strlen(attrib) - 1] = 0;

                     sprintf(value, loc("%s wrote"), attrib);
                     if (email_notify)
                        sprintf(return_buffer + j, email_quote_table, value);
                     else
                        sprintf(return_buffer + j, pattern_list[l].subst, value);
                     j += strlen(return_buffer + j);
                  } else {
                     if (email_notify)
                        sprintf(return_buffer + j, email_quote_table, loc("Quote"));
                     else
                        sprintf(return_buffer + j, pattern_list[l].subst, loc("Quote"));
                     j += strlen(return_buffer + j);
                     i += strlen(pattern_list[l].pattern) - 1;  // 1 gets added in for loop...
                  }
               } else if (strstr(pattern_list[l].subst, "%#")) {

                  /* special substitutions */
                  if (pattern_list[l].pattern[strlen(pattern_list[l].pattern) - 1] == '=') {

                     i += strlen(pattern_list[l].pattern);
                     strextract(str + i, ']', attrib, sizeof(attrib));
                     i += strlen(attrib) + 1;

                     if (strncmp(attrib, "elog:", 5) == 0) {    /* eval elog: */
                        strlcpy(tmp, attrib + 5, sizeof(tmp));
                        if (strchr(tmp, '/'))
                           *strchr(tmp, '/') = 0;

                        for (m = 0; m < (int) strlen(tmp); m++)
                           if (!isdigit(tmp[m]))
                              break;

                        if (m < (int) strlen(tmp))
                           /* if link contains reference to other logbook, add ".." in front */
                           sprintf(hattrib, "../%s", attrib + 5);
                        else if (attrib[5] == '/')
                           sprintf(hattrib, "%d%s", _current_message_id, attrib + 5);
                        else
                           sprintf(hattrib, "%s", attrib + 5);

                     } else if (strstr(attrib, "://") == 0 && attrib[0] != '#') {       /* add http:// if missing */
                        if (_ssl_flag)
                           sprintf(hattrib, "https://%s", attrib);
                        else
                           sprintf(hattrib, "http://%s", attrib);
                     } else
                        strlcpy(hattrib, attrib, sizeof(hattrib));

                     strextract(str + i, '[', value, sizeof(value));
                     i += strlen(value) - 1;
                     strlcpy(subst, pattern_list[l].subst, sizeof(subst));
                     *strchr(subst, '#') = 's';
                     sprintf(return_buffer + j, subst, hattrib, value);

                     j += strlen(return_buffer + j);

                  } else if (pattern_list[l].pattern[strlen(pattern_list[l].pattern) - 1] != '=') {

                     i += strlen(pattern_list[l].pattern);
                     strextract(str + i, '[', attrib, sizeof(attrib));
                     i += strlen(attrib) - 1;
                     strlcpy(hattrib, attrib, sizeof(hattrib));

                     /* replace elog:x/x for images */
                     if (strnieq(attrib, "elog:", 5)) {
                        if (email_notify) {
                           retrieve_domain(domain, sizeof(domain));
                           sprintf(hattrib, "cid:att%d@%s", m, domain);
                        } else {
                           if (email_notify)
                              compose_base_url(lbs, hattrib, sizeof(hattrib), TRUE);
                           else
                              hattrib[0] = 0;
                           if (attrib[5] == '/') {
                              if (_current_message_id == 0) {
                                 sprintf(param, "attachment%d", atoi(attrib + 6) - 1);
                                 if (isparam(param))
                                    strlcat(hattrib, getparam(param), sizeof(hattrib));
                              } else
                                 sprintf(hattrib + strlen(hattrib), "%d%s", _current_message_id, attrib + 5);
                           } else {
                              strlcat(hattrib, attrib + 5, sizeof(hattrib));
                           }
                        }
                     }

                        /* add http:// if missing */
                     else if ((!strnieq(attrib, "http://", 7) && !strnieq(attrib, "https://", 8)) &&
                              strstr(pattern_list[l].subst, "mailto") == NULL &&
                              strstr(pattern_list[l].subst, "img") == NULL &&
                              strncmp(pattern_list[l].subst, "<a", 2) != 0) {
                        if (_ssl_flag)
                           snprintf(hattrib, sizeof(hattrib), "https://%s", attrib);
                        else
                           snprintf(hattrib, sizeof(hattrib), "http://%s", attrib);
                     }

                     strlcpy(subst, pattern_list[l].subst, sizeof(subst));
                     if (email_notify && stristr(subst, "?thumb=1"))
                        strlcpy(subst, "<a href=\"%#\"><img border=0 src=\"%#\"></a>", sizeof(subst));
                     strsubst(subst, sizeof(subst), "%#", hattrib);
                     sprintf(return_buffer + j, subst, attrib);
                     j += strlen(return_buffer + j);
                  }

               } else if (pattern_list[l].pattern[strlen(pattern_list[l].pattern) - 1] == '=') {

                  /* extract sting after '=' and put it into '%s' of subst */
                  i += strlen(pattern_list[l].pattern);
                  strextract(str + i, ']', attrib, sizeof(attrib));
                  i += strlen(attrib);
                  sprintf(return_buffer + j, pattern_list[l].subst, attrib);
                  j += strlen(return_buffer + j);

               } else if (pattern_list[l].pattern[strlen(pattern_list[l].pattern) - 1] == ' ') {

                  /* extract sting after ' ' and put it into '%s' of subst */
                  i += strlen(pattern_list[l].pattern);
                  strextract(str + i, ']', attrib, sizeof(attrib));
                  i += strlen(attrib);
                  sprintf(return_buffer + j, pattern_list[l].subst, attrib);
                  j += strlen(return_buffer + j);

               } else if (strncmp(pattern_list[l].pattern, "[/list]", 7) == 0) {

                  if (ordered_list)
                     strcpy(subst, "</ol>");
                  else
                     strcpy(subst, "</ul>");
                  ordered_list = FALSE;

                  strcpy(return_buffer + j, subst);
                  j += strlen(subst);
                  i += strlen(pattern_list[l].pattern) - 1;     // 1 gets added in for loop...

               } else if (strncmp(pattern_list[l].pattern, "|", 1) == 0) {

                  if (inside_table) {
                     strcpy(link, pattern_list[l].subst);
                     if (strstr(link, "%s")) {
                        strcpy(tmp, link);
                        if (email_notify)
                           compose_base_url(lbs, base_url, sizeof(base_url), TRUE);
                        else
                           base_url[0] = 0;
                        sprintf(link, tmp, base_url);
                     }

                     strcpy(return_buffer + j, link);
                     j += strlen(link);
                     i += strlen(pattern_list[l].pattern) - 1;  // 1 gets added in for loop...
                  } else {
                     strcpy(return_buffer + j, pattern_list[l].pattern);
                     j += strlen(pattern_list[l].pattern);
                     i += strlen(pattern_list[l].pattern) - 1;  // 1 gets added in for loop...
                  }

               } else {

                  /* simple substitution */
                  strcpy(link, pattern_list[l].subst);
                  if (strstr(link, "%s")) {
                     strcpy(tmp, link);
                     if (email_notify)
                        compose_base_url(lbs, base_url, sizeof(base_url), TRUE);
                     else
                        base_url[0] = 0;
                     sprintf(link, tmp, base_url);
                  }

                  strcpy(return_buffer + j, link);
                  j += strlen(link);
                  i += strlen(pattern_list[l].pattern) - 1;     // 1 gets added in for loop...
               }
            }                   // !elcode_disabled && !elcode_disabled1

            else if (!elcode_disabled) {

               substituted = TRUE;

               /* simple substitution */
               strcpy(link, pattern_list[l].subst);
               if (strstr(link, "%s")) {
                  strcpy(tmp, link);
                  if (email_notify)
                     compose_base_url(lbs, base_url, sizeof(base_url), TRUE);
                  else
                     base_url[0] = 0;
                  sprintf(link, tmp, base_url);
               }

               strcpy(return_buffer + j, link);
               j += strlen(link);
               i += strlen(pattern_list[l].pattern) - 1;        // 1 gets added in for loop...
            }

            if (stristr(pattern_list[l].pattern, "[code]"))
               elcode_disabled = TRUE;
            if (stristr(pattern_list[l].pattern, "[code1]"))
               elcode_disabled1 = TRUE;

            break;
         }
      }

      /* don't output tags if already subsituted by HTML code */
      if (substituted)
         continue;

      if (strnieq(str + i, "<br>", 4)) {
         strcpy(return_buffer + j, "<br />");
         j += 6;
         i += 3;
      } else
         switch (str[i]) {
            case '\r':
               if (!elcode_disabled && !elcode_disabled1 && !inside_table) {
                  strcat(return_buffer, "<br />\r\n");
                  j += 8;
               } else {
                  strcat(return_buffer, "\r\n");
                  j += 2;
               }
               break;
            case '\n':
               break;
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

   xfree(lstr);
   return_buffer[j] = 0;
   strlen_retbuf = j;
}
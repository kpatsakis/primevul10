void show_standard_title(LOGBOOK *lbs, char *text, int printable) {
   char str[NAME_LENGTH], ref[256], sclass[32], comment[256], full_name[256], url[256], logbook[256];
   int i, j, level;
   LBLIST phier, pnode, pnext, flb;
   char slist[20][NAME_LENGTH], svalue[20][NAME_LENGTH];

   if (lbs == NULL)
      strlcpy(logbook, "global", sizeof(logbook));
   else
      strlcpy(logbook, lbs->name, sizeof(logbook));

   if (printable)
      rsprintf
              ("<table class=\"pframe\" cellpadding=\"0\" cellspacing=\"0\"><!-- show_standard_title -->\n\n");
   else
      rsprintf("<table class=\"frame\" cellpadding=\"0\" cellspacing=\"0\"><!-- show_standard_title -->\n\n");

   /* scan logbook hierarchy */
   phier = get_logbook_hierarchy();

   /*---- logbook selection row ----*/

   pnode = phier;               /* start at root of tree */
   pnext = NULL;

   if (!printable && (!getcfg(logbook, "logbook tabs", str, sizeof(str)) || atoi(str) == 1)) {

      for (level = 0;; level++) {
         rsprintf("<tr><td class=\"tabs\">\n");

         if (level == 0 && getcfg("global", "main tab", str, sizeof(str)) && !getcfg_topgroup()) {
            if (getcfg("global", "main tab url", url, sizeof(url)))
               rsprintf("<span class=\"ltab\"><a href=\"%s\">%s</a></span>\n", url, str);
            else
               rsprintf("<span class=\"ltab\"><a href=\"../\">%s</a></span>\n", str);
         }

         if (level == 1 && getcfg("global", "main tab", str, sizeof(str)) && getcfg_topgroup()) {
            if (getcfg("global", "main tab url", url, sizeof(url)))
               rsprintf("<span class=\"ltab\"><a href=\"%s/\">%s</a></span>\n", url, str);
            else
               rsprintf("<span class=\"ltab\"><a href=\"../%s/\">%s</a></span>\n", getcfg_topgroup(), str);
         }

         /* iterate through members of this group */
         for (i = 0; i < pnode->n_members; i++) {

            if (getcfg(pnode->member[i]->name, "Hidden", str, sizeof(str)) && atoi(str) == 1)
               continue;

            strlcpy(str, pnode->member[i]->name, sizeof(str));

            /* build reference to first logbook in group */
            comment[0] = 0;
            if (pnode->member[i]->member == NULL) {
               getcfg(pnode->member[i]->name, "Comment", comment, sizeof(comment));
               strlcpy(ref, str, sizeof(ref));  // current node is logbook
            } else {
               flb = pnode->member[i]->member[0];       // current node is group
               while (flb->member)
                  // so traverse hierarchy
                  flb = flb->member[0];
               strlcpy(ref, flb->name, sizeof(ref));
            }
            url_encode(ref, sizeof(ref));

            if (is_logbook_in_group(pnode->member[i], logbook)) {

               /* remember member list of this group for next row */
               pnext = pnode->member[i];

               if (pnode->member[i]->member == NULL)
                  /* selected logbook */
                  strcpy(sclass, "sltab");
               else
                  /* selected group */
                  strcpy(sclass, "sgtab");
            } else {
               if (pnode->member[i]->member == NULL)
                  /* unselected logbook */
                  strcpy(sclass, "ltab");
               else
                  /* unselected group */
                  strcpy(sclass, "gtab");
            }

            if (!pnode->member[i]->is_top) {

               rsprintf("<span class=\"%s\">", sclass);

               if (comment[0]) {
                  rsprintf("<a href=\"../%s/\" title=\"", ref);
                  rsputs3(comment);
                  rsprintf("\">");
               } else
                  rsprintf("<a href=\"../%s/\">", ref);

               strlcpy(str, pnode->member[i]->name, sizeof(str));

               for (j = 0; j < (int) strlen(str); j++)
                  if (str[j] == ' ')
                     rsprintf("&nbsp;");
                  else
                     rsprintf("%c", str[j]);

               rsprintf("</a></span>\n");
            }
         }

         rsprintf("</td></tr>\n\n");

         pnode = pnext;
         pnext = NULL;

         if (pnode == NULL || pnode->n_members == 0)
            break;
      }
   }

   free_logbook_hierarchy(phier);

   /*---- title row ----*/

   rsprintf("<tr><td><table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");

   /* left cell */
   rsprintf("<tr><td class=\"title1\">");

   /* use comment as title if available, else logbook name */
   if (!getcfg(logbook, "Comment", str, sizeof(str)))
      strcpy(str, logbook);

   rsprintf("&nbsp;&nbsp;");

   if (is_html(str))
      rsputs(str);
   else
      rsputs3(str);

   rsputs3(text);
   rsprintf("&nbsp;</td>\n");

   /* middle cell */
   if (isparam("unm")) {
      get_user_line(lbs, getparam("unm"), NULL, full_name, NULL, NULL, NULL, NULL);
      rsprintf("<td class=\"title2\">%s \"%s\"</td>\n", loc("Logged in as"), full_name);
   } else if (getcfg(lbs->name, "Guest menu commands", str, sizeof(str)))
      rsprintf("<td class=\"title2\" align=center>%s</td>\n", loc("Not logged in"));

   /* right cell */
   rsprintf("<td class=\"title3\">");

   if (getcfg(logbook, "Title image URL", str, sizeof(str)))
      rsprintf("<a href=\"%s\">\n", str);

   if (getcfg(logbook, "Title image", str, sizeof(str))) {
      // allow $short_name for example to link to personal pictures
      i = build_subst_list(lbs, slist, svalue, NULL, TRUE);
      strsubst_list(str, sizeof(str), slist, svalue, i);

      rsprintf("%s", str);
   } else
      rsprintf("<img border=0 src=\"elog.png\" alt=\"ELOG logo\" title=\"ELOG logo\">");

   if (getcfg(logbook, "Title image URL", str, sizeof(str)))
      rsprintf("</a>\n");

   rsprintf("</td>\n");

   rsprintf("</tr></table></td></tr>\n\n");
}
int check_drafts(LOGBOOK *lbs) {
   time_t now;
   char str[1000], draft[256], title[256], datetime[256], attrib[MAX_N_ATTR][NAME_LENGTH];
   int i, n_draft, *draft_id = NULL;

   /* if we got here already and user clicked "Create new entry", ignore is set and we skip this */
   if (isparam("ignore"))
      return 0;

   time(&now);
   /* check if any recent draft */
   for (i = n_draft = 0; i < *(lbs->n_el_index); i++)
      if (lbs->el_index[i].file_time > now - 3600 * 24 * 7) { // only one week to reduce seek time
         el_retrieve(lbs, lbs->el_index[i].message_id, NULL, attr_list, attrib, lbs->n_attr,
                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, draft);
         if (draft[0] && is_author(lbs, attrib, draft)) {
            if (n_draft == 0)
               draft_id = (int *) xmalloc(sizeof(int));
            else
               draft_id = (int *) xrealloc(draft_id, sizeof(int) * (n_draft + 1));
            draft_id[n_draft] = lbs->el_index[i].message_id;
            n_draft++;
         }
      }

   if (n_draft == 0)
      return 0;

   if (n_draft == 1)
      sprintf(title, "%s", loc("Pending draft available"));
   else
      sprintf(title, loc("%d pending drafts available"), n_draft);

   show_standard_header(lbs, TRUE, "Draft query", NULL, FALSE, NULL, NULL, 0);

   rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>\n");

   rsprintf("<tr><td colspan=\"2\" class=\"dlgtitle\">");
   rsprintf("%s</td></tr>\n", title);

   for (i = 0; i < n_draft; i++) {
      el_retrieve(lbs, draft_id[i], datetime, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL,
                  NULL, NULL, draft);

      rsprintf("<tr><td class=\"draftsel\" align=\"center\">");
      sprintf(str, loc("Draft entry created on %s by %s"), datetime, draft);
      rsprintf("%s</td>\n", str);
      rsprintf("<td class=\"draftsel\">");
      rsprintf("<input type=button value=\"%s\" onClick=\"window.location.href='%d?cmd=%s';\">",
               loc("Edit"), draft_id[i], loc("Edit"));
      rsprintf("</td></tr>\n");
   }

   rsprintf("<tr><td colspan=\"2\" align=center class=\"dlgform\">");
   rsprintf("<input type=button value=\"%s\" onClick=\"window.location.href='?cmd=%s&ignore=1';\">\n",
            loc("Create new entry"),
            loc("New"));
   rsprintf("</td></tr>\n\n");

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");

   xfree(draft_id);
   return 1;
}
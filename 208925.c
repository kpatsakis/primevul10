void show_elog_thread(LOGBOOK *lbs, int message_id, int absolute_links, int highlight_mid) {
   int size, head_id, n_display, n_attr_disp;
   char date[80], attrib[MAX_N_ATTR][NAME_LENGTH], *text, in_reply_to[80], reply_to[MAX_REPLY_TO * 10],
           attachment[MAX_ATTACHMENTS][MAX_PATH_LENGTH], encoding[80], locked_by[256], draft[256],
           disp_attr[MAX_N_ATTR + 4][NAME_LENGTH];
   char *p;

   text = (char *) xmalloc(TEXT_SIZE);

   /* retrieve message */
   size = TEXT_SIZE;
   el_retrieve(lbs, message_id, date, attr_list, attrib, lbs->n_attr, text, &size, in_reply_to,
               reply_to, attachment, encoding, locked_by, draft);

   /* find message head */
   if (atoi(in_reply_to))
      head_id = find_thread_head(lbs, atoi(in_reply_to));
   else
      head_id = message_id;

   n_attr_disp = lbs->n_attr + 2;
   strcpy(disp_attr[0], loc("ID"));
   strcpy(disp_attr[1], loc("Date"));
   memcpy(disp_attr + 2, attr_list, sizeof(attr_list));

   size = TEXT_SIZE;
   el_retrieve(lbs, head_id, date, attr_list, attrib, lbs->n_attr, text, &size, in_reply_to,
               reply_to, attachment, encoding, locked_by, draft);

   rsprintf("<tr><td><table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");

   display_line(lbs, head_id, 0, "Threaded", 1, 0, FALSE, 0,
                FALSE, FALSE, date, in_reply_to, reply_to, n_attr_disp, disp_attr, NULL, attrib, lbs->n_attr,
                text, FALSE, attachment, encoding, 0, &n_display, locked_by, message_id, NULL, highlight_mid,
                absolute_links, draft);

   if (reply_to[0]) {
      p = reply_to;
      do {
         display_reply(lbs, atoi(p), FALSE, 1, 0, n_attr_disp, disp_attr, FALSE, 1, message_id, NULL,
                       highlight_mid, absolute_links);

         while (*p && isdigit(*p))
            p++;
         while (*p && (*p == ',' || *p == ' '))
            p++;
      } while (*p);
   }

   rsprintf("</table>\n");
   rsprintf("</td></tr>\n");

   xfree(text);
}
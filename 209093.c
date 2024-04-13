void display_reply(LOGBOOK *lbs, int message_id, int printable, int expand, int n_line, int n_attr_disp,
                   char disp_attr[MAX_N_ATTR + 4][NAME_LENGTH], BOOL show_text, int level, int highlight,
                   regex_t *re_buf, int highlight_mid, int absolute_link) {
   char *date, *text, *in_reply_to, *reply_to, *encoding, *attachment, *locked_by, *draft, *attrib, *p;
   int status, size;

   text = (char *) xmalloc(TEXT_SIZE);
   attachment = (char *) xmalloc(MAX_PATH_LENGTH * MAX_ATTACHMENTS);
   attrib = (char *) xmalloc(MAX_N_ATTR * NAME_LENGTH);
   date = (char *) xmalloc(80);
   in_reply_to = (char *) xmalloc(80);
   reply_to = (char *) xmalloc(256);
   encoding = (char *) xmalloc(80);
   locked_by = (char *) xmalloc(256);
   draft = (char *) xmalloc(256);

   if (draft == NULL)
      return;

   reply_to[0] = 0;
   size = TEXT_SIZE;
   status = el_retrieve(lbs, message_id, date, attr_list, (char (*)[1500]) attrib, lbs->n_attr, text, &size,
                        in_reply_to, reply_to, (char (*)[256]) attachment, encoding, locked_by, draft);

   if (status != EL_SUCCESS || draft[0]) {
      xfree(text);
      xfree(attachment);
      xfree(attrib);
      xfree(date);
      xfree(in_reply_to);
      xfree(reply_to);
      xfree(encoding);
      xfree(locked_by);
      xfree(draft);
      return;
   }

   display_line(lbs, message_id, 0, "threaded", expand, level, printable, n_line, FALSE, FALSE, date,
                in_reply_to, reply_to, n_attr_disp, disp_attr, NULL, (char (*)[1500]) attrib, lbs->n_attr,
                text, show_text, (char (*)[256]) attachment, encoding, 0, NULL, locked_by, highlight,
                &re_buf[0], highlight_mid, absolute_link, draft);

   if (reply_to[0]) {
      p = reply_to;
      do {
         display_reply(lbs, atoi(p), printable, expand, n_line, n_attr_disp, disp_attr, show_text, level + 1,
                       highlight, &re_buf[0], highlight_mid, absolute_link);

         while (*p && isdigit(*p))
            p++;
         while (*p && (*p == ',' || *p == ' '))
            p++;
      } while (*p);
   }

   xfree(text);
   xfree(attachment);
   xfree(attrib);
   xfree(date);
   xfree(in_reply_to);
   xfree(reply_to);
   xfree(encoding);
   xfree(locked_by);
   xfree(draft);
}
void copy_to(LOGBOOK *lbs, int src_id, char *dest_logbook, int move, int orig_id) {
   int size, i, j, n, n_done, n_done_reply, n_reply, index, status, fh, source_id, message_id,
           thumb_status, next_id = 0;
   char str[256], str2[256], file_name[MAX_PATH_LENGTH], thumb_name[MAX_PATH_LENGTH],
           *attrib, date[80], *text, msg_str[32], in_reply_to[80], subdir[256],
           reply_to[MAX_REPLY_TO * 10], *attachment, encoding[80], locked_by[256], draft[256],
           *buffer, *list;
   LOGBOOK *lbs_dest;
   BOOL bedit;

   attachment = xmalloc(MAX_ATTACHMENTS * MAX_PATH_LENGTH);
   attrib = xmalloc(MAX_N_ATTR * NAME_LENGTH);
   list = xmalloc(MAX_N_ATTR * NAME_LENGTH);
   text = xmalloc(TEXT_SIZE);

   for (i = 0; lb_list[i].name[0]; i++)
      if (strieq(lb_list[i].name, dest_logbook))
         break;
   if (!lb_list[i].name[0])
      return;
   lbs_dest = &lb_list[i];

   if (src_id)
      n = 1;
   else
      n = isparam("nsel") ? atoi(getparam("nsel")) : 0;

   n_done = n_done_reply = source_id = status = next_id = 0;
   for (index = 0; index < n; index++) {
      if (src_id)
         source_id = src_id;
      else {
         sprintf(str, "s%d", index);
         if (!isparam(str))
            continue;

         source_id = isparam(str) ? atoi(getparam(str)) : 0;
      }

      /* get message */
      size = TEXT_SIZE;
      status =
              el_retrieve(lbs, source_id, date, attr_list, (void *) attrib, lbs->n_attr, text, &size,
                          in_reply_to,
                          reply_to, (void *) attachment, encoding, locked_by, draft);

      if (status != EL_SUCCESS) {
         sprintf(msg_str, "%d", source_id);
         sprintf(str, loc("Entry %s cannot be read from logbook \"%s\""), msg_str, lbs->name);
         show_error(str);
         xfree(attachment);
         xfree(attrib);
         xfree(list);
         xfree(text);
         return;
      }

      if (orig_id == 0) {
         /* search message head */
         while (atoi(in_reply_to) > 0) {
            source_id = atoi(in_reply_to);
            size = TEXT_SIZE;
            status = el_retrieve(lbs, source_id, date, attr_list, (void *) attrib, lbs->n_attr, text, &size,
                                 in_reply_to, reply_to, (void *) attachment, encoding, locked_by, draft);

            if (status != EL_SUCCESS) {
               sprintf(msg_str, "%d", source_id);
               sprintf(str, loc("Entry %s cannot be read from logbook \"%s\""), msg_str, lbs->name);
               show_error(str);
               xfree(attachment);
               xfree(attrib);
               xfree(list);
               xfree(text);
               return;
            }
         }
      }

      /* read attachments */
      for (i = 0; i < MAX_ATTACHMENTS; i++)
         if (attachment[i * MAX_PATH_LENGTH]) {
            strlcpy(file_name, lbs->data_dir, sizeof(file_name));
            generate_subdir_name(attachment + i * MAX_PATH_LENGTH, subdir, sizeof(subdir));
            strlcat(file_name, subdir, sizeof(file_name));
            strlcat(file_name, attachment + i * MAX_PATH_LENGTH, sizeof(file_name));

            fh = open(file_name, O_RDONLY | O_BINARY);
            if (fh > 0) {
               lseek(fh, 0, SEEK_END);
               size = TELL(fh);
               lseek(fh, 0, SEEK_SET);
               buffer = xmalloc(size);
               read(fh, buffer, size);
               close(fh);

               /* keep original file name for inline references */
               strlcpy(file_name, attachment + i * MAX_PATH_LENGTH, MAX_PATH_LENGTH);

               el_submit_attachment(lbs_dest, file_name, buffer, size, NULL);

               if (buffer)
                  xfree(buffer);
            } else
               /* attachment is invalid */
               attachment[i * MAX_PATH_LENGTH] = 0;

            /* check for thumbnail */
            strlcpy(file_name, lbs->data_dir, sizeof(file_name));
            generate_subdir_name(attachment + i * MAX_PATH_LENGTH, subdir, sizeof(subdir));
            strlcat(file_name, subdir, sizeof(file_name));
            strlcat(file_name, attachment + i * MAX_PATH_LENGTH, sizeof(file_name));
            thumb_status = get_thumb_name(file_name, thumb_name, sizeof(thumb_name), 0);

            if (thumb_status == 1) {
               fh = open(thumb_name, O_RDONLY | O_BINARY);
               if (fh > 0) {
                  lseek(fh, 0, SEEK_END);
                  size = TELL(fh);
                  lseek(fh, 0, SEEK_SET);
                  buffer = xmalloc(size);
                  read(fh, buffer, size);
                  close(fh);

                  /* keep original file name for inline references */
                  if (strrchr(thumb_name, '\\'))
                     strlcpy(str, strrchr(thumb_name, '\\') + 1, sizeof(str));
                  else
                     strlcpy(str, thumb_name, sizeof(str));

                  el_submit_attachment(lbs_dest, str, buffer, size, NULL);

                  if (buffer)
                     xfree(buffer);
               }
            }
            if (thumb_status == 2) {
               for (j = 0;; j++) {
                  get_thumb_name(file_name, thumb_name, sizeof(thumb_name), j);
                  if (thumb_name[0]) {
                     fh = open(thumb_name, O_RDONLY | O_BINARY);
                     if (fh > 0) {
                        lseek(fh, 0, SEEK_END);
                        size = TELL(fh);
                        lseek(fh, 0, SEEK_SET);
                        buffer = xmalloc(size);
                        read(fh, buffer, size);
                        close(fh);

                        /* keep original file name for inline references */
                        if (strrchr(thumb_name, '\\'))
                           strlcpy(str, strrchr(thumb_name, '\\') + 1, sizeof(str));
                        else
                           strlcpy(str, thumb_name, sizeof(str));

                        el_submit_attachment(lbs_dest, str, buffer, size, NULL);

                        if (buffer)
                           xfree(buffer);
                     }
                  } else
                     break;
               }
            }
         }

      /* correct possible references to attachments */
      if (strieq(encoding, "ELCode")) {
         sprintf(str, "[IMG]elog:%d/", src_id);
         while (stristr(text, str))
            strsubst(text, TEXT_SIZE, str, "[IMG]elog:/");
      } else if (strieq(encoding, "HTML")) {
         sprintf(str, "?lb=%s\"", lbs->name_enc);
         sprintf(str2, "?lb=%s\"", dest_logbook);
         while (stristr(text, str))
            strsubst(text, TEXT_SIZE, str, str2);
         sprintf(str, "?lb=%s&", lbs->name_enc);
         sprintf(str2, "?lb=%s&", dest_logbook);
         while (stristr(text, str))
            strsubst(text, TEXT_SIZE, str, str2);
      }

      /* keep original message ID if requested */
      message_id = 0;
      bedit = FALSE;
      if (getcfg(lbs->name, "Preserve IDs", str, sizeof(str)) && atoi(str) == 1) {
         message_id = source_id;

         /* test if entry exists already */
         status =
                 el_retrieve(lbs_dest, message_id, NULL, NULL, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL,
                             NULL);
         bedit = (status == EL_SUCCESS);
      }

      /* submit in destination logbook without links, submit all attributes from
         the source logbook even if the destination has a differnt number of attributes */

      if (getcfg(lbs->name, "Preserve IDs", str, sizeof(str)) && atoi(str) == 1)
         message_id =
                 el_submit(lbs_dest, message_id, bedit, date, attr_list, (void *) attrib, lbs->n_attr, text,
                           in_reply_to, reply_to, encoding, (void *) attachment, FALSE, NULL, NULL);
      else {
         /* if called recursively (for threads), put in correct in_reply_to */
         str[0] = 0;
         if (orig_id)
            sprintf(str, "%d", orig_id);

         message_id =
                 el_submit(lbs_dest, message_id, bedit, date, attr_list, (void *) attrib, lbs->n_attr, text,
                           str,
                           "", encoding, (void *) attachment, TRUE, NULL, NULL);
      }

      if (message_id <= 0) {
         sprintf(str, loc("New entry cannot be written to directory \"%s\""), lbs_dest->data_dir);
         strcat(str, "\n<p>");
         strcat(str, loc("Please check that it exists and elogd has write access"));
         show_error(str);
         xfree(attachment);
         xfree(attrib);
         xfree(list);
         xfree(text);
         return;
      }

      n_done++;

      /* submit all replies */
      n_reply = strbreak(reply_to, (void *) list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < n_reply; i++) {
         copy_to(lbs, atoi(list + i * NAME_LENGTH), dest_logbook, move, message_id);
      }

      n_done_reply += n_reply;

      /* delete original message for move */
      next_id = source_id;
      if (move && orig_id == 0) {

         /* find next message head */
         next_id = el_search_message(lbs, EL_NEXT, source_id, TRUE);
         if (next_id <= 0)
            next_id = el_search_message(lbs, EL_LAST, 0, FALSE);

         el_delete_message(lbs, source_id, TRUE, NULL, TRUE, TRUE);
      }
   }

   xfree(attachment);
   xfree(attrib);
   xfree(list);
   xfree(text);

   if (orig_id)
      return;

   /* redirect to next entry of source logbook */
   if (next_id)
      sprintf(str, "%d", next_id);
   else
      str[0] = 0;
   redirect(lbs, str);
   return;
}
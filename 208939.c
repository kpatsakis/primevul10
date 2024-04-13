void show_elog_delete(LOGBOOK *lbs, int message_id) {
   int i, status, reply = 0, next, nsel;
   char str[256], str2[256], in_reply_to[80], reply_to[MAX_REPLY_TO * 10], owner[256];
   char attrib[MAX_N_ATTR][NAME_LENGTH], mode[80];

   /* check for editing interval */
   if (isparam("nsel")) {
      for (i = 0; i < atoi(getparam("nsel")); i++) {
         sprintf(str, "s%d", i);
         if (isparam(str)) {
            status = check_edit_time(lbs, atoi(getparam(str)));
            if (!status) {
               return;
            }
         }
      }
   } else if (message_id) {
      status = check_edit_time(lbs, message_id);
      if (!status) {
         return;
      }
   }

   /* redirect if confirm = NO */
   if (isparam("confirm") && strcmp(getparam("confirm"), loc("No")) == 0) {
      if (message_id) {
         sprintf(str, "%d", message_id);
         redirect(lbs, str);
      } else {
         strlcpy(str, isparam("lastcmd") ? getparam("lastcmd") : "", sizeof(str));
         url_decode(str);
         redirect(lbs, str);
      }
      return;
   }

   if (isparam("elmode"))
      strlcpy(mode, getparam("elmode"), sizeof(mode));

   if (isparam("confirm")) {
      if (strcmp(getparam("confirm"), loc("Yes")) == 0) {
         if (message_id) {
            /* delete message */
            status = el_delete_message(lbs, message_id, TRUE, NULL, TRUE, TRUE);
            if (status != EL_SUCCESS) {
               sprintf(str, "%s = %d", loc("Error deleting message: status"), status);
               show_error(str);
               return;
            } else {
               strlcpy(str, isparam("nextmsg") ? getparam("nextmsg") : "", sizeof(str));
               if (atoi(str) == 0)
                  sprintf(str, "%d", el_search_message(lbs, EL_LAST, 0, TRUE));
               if (atoi(str) == 0)
                  redirect(lbs, "");
               else
                  redirect(lbs, str);
               return;
            }
         } else {
            if (isparam("nsel")) {
               for (i = 0; i < atoi(getparam("nsel")); i++) {
                  sprintf(str, "s%d", i);
                  if (isparam(str)) {
                     if (strieq(mode, "threaded"))
                        status = el_delete_message(lbs, atoi(getparam(str)), TRUE, NULL, TRUE, TRUE);
                     else
                        status = el_delete_message(lbs, atoi(getparam(str)), TRUE, NULL, TRUE, FALSE);
                  }
               }
            }

            redirect(lbs, isparam("lastcmd") ? getparam("lastcmd") : "");
            return;
         }
      }
   } else {
      /* check if at least one message is selected */
      if (!message_id) {
         nsel = isparam("nsel") ? atoi(getparam("nsel")) : 0;
         for (i = 0; i < nsel; i++) {
            sprintf(str, "s%d", i);
            if (isparam(str))
               break;
         }
         if (i == nsel) {
            show_error(loc("No entry selected for deletion"));
            return;
         }
      }

      /* check for author */
      if (getcfg(lbs->name, "Restrict edit", str, sizeof(str)) && atoi(str) == 1) {
         /* get message for reply/edit */

         el_retrieve(lbs, message_id, NULL, attr_list, attrib, lbs->n_attr, NULL, NULL,
                     NULL, NULL, NULL, NULL, NULL, NULL);

         if (!is_author(lbs, attrib, owner)) {
            strencode2(str2, owner, sizeof(str2));
            sprintf(str, loc("Only user <b>%s</b> can delete this entry"), str2);
            show_error(str);
            return;
         }
      }

      /* header */
      if (message_id)
         sprintf(str, "%d", message_id);
      else
         str[0] = 0;
      show_standard_header(lbs, TRUE, "Delete ELog entry", str, FALSE, NULL, NULL, 0);

      rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");
      rsprintf("<tr><td class=\"dlgtitle\">\n");

      /* define hidden field for command */
      rsprintf("<input type=hidden name=cmd value=\"%s\">\n", loc("Delete"));

      if (!message_id) {
         rsprintf("%s</td></tr>\n", loc("Are you sure to delete these messages?"));

         rsprintf("<tr><td align=center class=\"dlgform\">\n");
         if (isparam("nsel"))
            rsprintf("<input type=hidden name=nsel value=%s>\n", getparam("nsel"));

         if (isparam("lastcmd")) {
            strlcpy(str, getparam("lastcmd"), sizeof(str));
            rsprintf("<input type=hidden name=lastcmd value=\"%s\">\n", str);
         }

         if (isparam("nsel")) {
            reply = FALSE;
            for (i = 0; i < atoi(getparam("nsel")); i++) {
               sprintf(str, "s%d", i);
               if (isparam(str)) {
                  rsprintf("#%s ", getparam(str));
                  rsprintf("<input type=hidden name=%s value=%s>\n", str, getparam(str));
               }

               if (!reply) {
                  el_retrieve(lbs, isparam(str) ? atoi(getparam(str)) : 0,
                              NULL, attr_list, NULL, 0, NULL, NULL, in_reply_to, reply_to, NULL, NULL, NULL,
                              NULL);
                  if (reply_to[0])
                     reply = TRUE;
               }
            }
         }

         rsprintf("</td></tr>\n");

         if (reply && strieq(mode, "threaded"))
            rsprintf("<tr><td align=center class=\"dlgform\">%s</td></tr>\n", loc("and all their replies"));

      } else {
         rsprintf("%s</td></tr>\n", loc("Are you sure to delete this entry?"));

         /* check for replies */

         /* retrieve original message */
         el_retrieve(lbs, message_id, NULL, attr_list, NULL, 0, NULL, NULL, in_reply_to, reply_to, NULL,
                     NULL, NULL, NULL);

         if (reply_to[0])
            rsprintf("<tr><td align=center class=\"dlgform\">#%d<br>%s</td></tr>\n", message_id,
                     loc("and all its replies"));
         else
            rsprintf("<tr><td align=center class=\"dlgform\">#%d</td></tr>\n", message_id);

         /* put link to next message */
         next = el_search_message(lbs, EL_NEXT, message_id, TRUE);

         rsprintf("<input type=hidden name=nextmsg value=%d>\n", next);
      }

      rsprintf("<tr><td align=center class=\"dlgform\"><input type=submit name=confirm value=\"%s\">\n",
               loc("Yes"));
      rsprintf("<input type=submit name=confirm value=\"%s\">\n", loc("No"));
      rsprintf("</td></tr>\n\n");
   }

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</body></html>\r\n");
}
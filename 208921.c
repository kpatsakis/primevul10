void synchronize(LOGBOOK *lbs, int mode) {
   int i;
   char str[256], pwd[256];

   if (mode == SYNC_HTML) {
      show_html_header(NULL, FALSE, loc("Synchronization"), TRUE, FALSE, NULL, FALSE, 0);
      rsprintf("<body>\n");
   }

   if (lbs == NULL) {
      for (i = 0; lb_list[i].name[0]; i++)
         if (getcfg(lb_list[i].name, "mirror server", str, sizeof(str))) {

            if (exist_top_group() && getcfg_topgroup())
               if (lb_list[i].top_group[0] && !strieq(lb_list[i].top_group, getcfg_topgroup()))
                  continue;

            /* skip if excluded */
            if (getcfg(lb_list[i].name, "Mirror exclude", str, sizeof(str)) && atoi(str) == 1)
               continue;

            /* if called by cron, set user name and password */
            if (mode == SYNC_CRON && getcfg(lb_list[i].name, "mirror user", str, sizeof(str))) {
               if (get_user_line(&lb_list[i], str, pwd, NULL, NULL, NULL, NULL, NULL) == EL_SUCCESS) {
                  setparam("unm", str);
                  setparam("upwd", pwd);
               }
            }

            synchronize_logbook(&lb_list[i], mode, TRUE);
         }
   } else
      synchronize_logbook(lbs, mode, FALSE);

   if (mode == SYNC_HTML) {
      rsprintf("<table width=\"100%%\" cellpadding=\"1\" cellspacing=\"0\"");
      rsprintf("<tr><td class=\"seltitle\"><a href=\".\">%s</a></td></tr>\n", loc("Back"));
      rsprintf("</table><p>\n");

      rsprintf("</body></html>\n");
      flush_return_buffer();
      keep_alive = FALSE;
   }
}
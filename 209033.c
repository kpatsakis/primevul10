int activate_user(LOGBOOK *lbs, char *user_name, int code) {
   int inactive, self_register;
   char str[256], str2[256], smtp_host[256], url[256], mail_text[2000],
           error[256], mail_from_name[256], mail_from[256], user_email[256], logbook[256];

   if (lbs == NULL)
      strlcpy(logbook, "global", sizeof(logbook));
   else
      strlcpy(logbook, lbs->name, sizeof(logbook));

   get_user_line(lbs, user_name, NULL, NULL, user_email, NULL, NULL, &inactive);
   if (code != inactive) {
      show_error(loc("Invalid activation code"));
      return FALSE;
   }

   if (!set_user_inactive(lbs, user_name, 0)) {
      show_error(loc("Error activating user"));
      return FALSE;
   }

   self_register = 0;
   if (getcfg(logbook, "Self register", str, sizeof(str)))
      self_register = atoi(str);

   if (self_register == 3) {

      if (!getcfg("global", "SMTP host", smtp_host, sizeof(smtp_host))) {
         show_error(loc("No SMTP host defined in [global] section of configuration file"));
         return FALSE;
      }

      /* try to get URL from referer */
      if (!getcfg("global", "URL", url, sizeof(url))) {
         if (referer[0])
            strcpy(url, referer);
         else {
            if (elog_tcp_port == 80) {
               if (_ssl_flag)
                  sprintf(url, "https://%s/", http_host);
               else
                  sprintf(url, "http://%s/", http_host);
            } else {
               if (_ssl_flag)
                  sprintf(url, "https://%s:%d/", http_host, elog_tcp_port);
               else
                  sprintf(url, "http://%s:%d/", http_host, elog_tcp_port);
            }
            if (lbs) {
               strlcat(url, lbs->name_enc, sizeof(url));
               strlcat(url, "/", sizeof(url));
            }
         }
      } else {
         if (url[strlen(url) - 1] != '/')
            strlcat(url, "/", sizeof(url));
         if (lbs) {
            strlcat(url, lbs->name_enc, sizeof(url));
            strlcat(url, "/", sizeof(url));
         }
      }

      retrieve_email_from(lbs, mail_from, mail_from_name, NULL);

      mail_text[0] = 0;
      compose_email_header(lbs, loc("Your ELOG account has been activated"), mail_from_name,
                           user_email, NULL, mail_text, sizeof(mail_text), 1, 0, NULL, 0, 0);

      strlcat(mail_text, "\r\n", sizeof(mail_text));
      strlcat(mail_text, loc("Your ELOG account has been activated on host"), sizeof(mail_text));
      sprintf(mail_text + strlen(mail_text), " %s", http_host);
      sprintf(mail_text + strlen(mail_text), ".\r\n\r\n");
      sprintf(url + strlen(url), "?unm=%s", user_name);
      sprintf(mail_text + strlen(mail_text), "%s %s.\r\n\r\n", loc("You can access it at"), url);
      sprintf(mail_text + strlen(mail_text), "%s.\r\n",
              loc("To subscribe to any logbook, click on 'Config' in that logbook"));

      if (sendmail(lbs, smtp_host, mail_from, user_email, mail_text, error, sizeof(error)) == -1) {
         sprintf(str, loc("Cannot send email notification to \"%s\""), user_email);
         strlcat(str, " : ", sizeof(str));
         strlcat(str, error, sizeof(str));
         strencode2(str2, str, sizeof(str2));
         show_error(str2);
         return FALSE;
      }
   }

   return TRUE;
}
void show_forgot_pwd_page(LOGBOOK *lbs) {
   int i;
   char str[1000], str2[1000], login_name[256], full_name[256], user_email[256], name[256], pwd[256],
           redir[256], smtp_host[256], mail_from[256], mail_from_name[256], subject[256],
           mail_text[1000], url[1000], error[1000];

   if (isparam("login_name")) {
      /* seach in pwd file */

      strcpy(name, getparam("login_name"));

      for (i = 0;; i++) {
         if (!enum_user_line(lbs, i, login_name, sizeof(login_name)))
            break;

         get_user_line(lbs, login_name, NULL, full_name, user_email, NULL, NULL, NULL);

         if (strieq(name, login_name) || strieq(name, full_name) || strieq(name, user_email)) {
            if (user_email[0] == 0) {
               sprintf(str, loc("No Email address registered with user name <i>\"%s\"</i>"), name);
               strencode2(str2, str, sizeof(str2));
               show_error(str2);
               return;
            }

            /* create random password */
            for (i = 0; i < 16; i++)
               pwd[i] = 'A' + (rand() % 25);
            pwd[i] = 0;

            /* send email with new password */
            if (!getcfg("global", "SMTP host", smtp_host, sizeof(smtp_host))) {
               show_error(loc("No SMTP host defined in [global] section of configuration file"));
               return;
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
               }
            } else {
               if (url[strlen(url) - 1] != '/')
                  strlcat(url, "/", sizeof(url));
               if (lbs) {
                  strlcat(url, lbs->name_enc, sizeof(url));
                  strlcat(url, "/", sizeof(url));
               }
            }

            url_slash_encode(pwd, sizeof(pwd));
            sprintf(redir, "?cmd=%s&oldpwd=%s", loc("Change password"), pwd);
            url_encode(redir, sizeof(redir));

            strencode2(str2, redir, sizeof(str2));
            sprintf(str, "?redir=%s&uname=%s&upassword=%s", str2, login_name, pwd);
            strlcat(url, str, sizeof(url));

            retrieve_email_from(lbs, mail_from, mail_from_name, NULL);

            if (lbs)
               sprintf(subject, loc("Password recovery for ELOG %s"), lbs->name);
            else
               sprintf(subject, loc("Password recovery for ELOG %s"), http_host);

            mail_text[0] = 0;
            compose_email_header(lbs, subject, mail_from_name, user_email, NULL, mail_text,
                                 sizeof(mail_text), 1, 0, NULL, 0, 0);

            strlcat(mail_text, "\r\n", sizeof(mail_text));
            sprintf(mail_text + strlen(mail_text),
                    loc("This is an automatically generated account recovery email for host %s"), http_host);
            strlcat(mail_text, ".\r\n", sizeof(mail_text));
            strlcat(mail_text, loc("Please click on following link to recover your account"),
                    sizeof(mail_text));
            strlcat(mail_text, ":\r\n\r\n", sizeof(mail_text));
            strlcat(mail_text, url, sizeof(mail_text));
            strlcat(mail_text, "\r\n\r\n", sizeof(mail_text));
            sprintf(mail_text + strlen(mail_text), "ELOG Version %s\r\n", VERSION);

            if (sendmail(lbs, smtp_host, mail_from, user_email, mail_text, error, sizeof(error)) != -1) {
               /* save new password */
               auth_change_password(lbs, login_name, NULL, pwd, str, sizeof(str));

               /* show notification web page */
               show_standard_header(lbs, FALSE, loc("ELOG password recovery"), "", FALSE, NULL, NULL, 0);

               rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");
               rsprintf("<tr><td class=\"dlgtitle\">\n");
               rsprintf(loc("Email notification"));
               rsprintf("</td></tr>\n");

               rsprintf("<tr><td align=center class=\"dlgform\">\n");
               rsprintf(loc("A password recovery email for user <i>\"%s\"</i> has been sent to %s"),
                        full_name, user_email);
               rsprintf("</td></tr></table>\n");
               show_bottom_text(lbs);
               rsprintf("</body></html>\n");
               return;
            } else {
               sprintf(str, loc("Error sending Email via \"%s\""), smtp_host);
               strlcat(str, ": ", sizeof(str));
               strlcat(str, error, sizeof(str));
               show_error(str);
               return;
            }
         }
      }

      if (strchr(name, '@'))
         sprintf(str, loc("Email address \"%s\" not registered"), name);
      else
         sprintf(str, loc("User name \"%s\" not registered"), name);

      strencode2(str2, str, sizeof(str2));
      show_error(str2);

      return;
   } else {
      /*---- header ----*/

      getcfg(lbs->name, "Authentication", str, sizeof(str));
      if (stristr(str, "Kerberos") || stristr(str, "Webserver") || stristr(str, "PAM")) {
         show_error
                 ("This installation of ELOG uses site authentication\nwhere password recovery is not possible");
         return;
      }

      show_standard_header(lbs, TRUE, loc("ELOG password recovery"), NULL, FALSE, NULL, NULL, 0);

      rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");

      /*---- entry form ----*/

      rsprintf("<tr><td class=\"dlgtitle\">%s</td></tr>\n", loc("Enter your user name or email address"));

      rsprintf("<tr><td align=center class=\"dlgform\">\n");
      rsprintf("<input type=hidden name=cmd value=%s>\n", loc("Forgot"));
      rsprintf("<input type=text size=40 name=login_name></td></tr>\n");

      rsprintf("<tr><td align=center class=\"dlgform\"><input type=submit value=\"%s\">\n", loc("Submit"));

      rsprintf("</td></tr></table>\n\n");
      show_bottom_text(lbs);
      rsprintf("</form></body></html>\r\n");
   }
}
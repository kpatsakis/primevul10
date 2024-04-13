int save_user_config(LOGBOOK *lbs, char *user, BOOL new_user) {
   char file_name[256], str[1000], *pl, user_enc[256], new_pwd[80], new_pwd2[80], smtp_host[256],
           email_addr[256], mail_from[256], mail_from_name[256], subject[256], mail_text[2000], str2[256],
           admin_user[80], url[256], error[2000], sid[32];
   int i, self_register, code, first_user;
   PMXML_NODE node, subnode, npwd;

   /* if we outsourced the authentication, use external username */
   getcfg(lbs->name, "Authentication", str, sizeof(str));
   if (stristr(str, "Webserver")) {
      /* do not allow HTML in user name */
      strencode2(user_enc, http_user, sizeof(user_enc));
   } else {
      strencode2(user_enc, user, sizeof(user_enc));
   }

   /* check for user name */
   if (!isparam("new_user_name") || *getparam("new_user_name") == 0) {
      sprintf(str, loc("Please enter \"%s\""), loc("Login name"));
      show_error(str);
      return 0;
   }

   /* check for full name */
   if (!isparam("new_full_name") || *getparam("new_full_name") == 0) {
      sprintf(str, loc("Please enter \"%s\""), loc("Full name"));
      show_error(str);
      return 0;
   }

   /* check for valid email address */
   if (!isparam("new_user_email") || *getparam("new_user_email") == 0) {
      show_error(loc("Please specify a valid email address"));
      return 0;
   }
   strlcpy(str, getparam("new_user_email"), sizeof(str));
   if (strchr(str, '@') == NULL || strchr(str, '.') == NULL) {
      show_error(loc("Please specify a valid email address"));
      return 0;
   }

   /* check for blank character in user name */
   strlcpy(str, getparam("new_user_name"), sizeof(str));
   if (strchr(str, ' ')) {
      show_error(loc("User name may not contain blanks"));
      return 0;
   }

   /* check for blank password if not external authentication */
   if (isparam("newpwd")) {
      strlcpy(str, getparam("newpwd"), sizeof(str));
      if (str[0] == 0) {
         show_error(loc("Empty password not allowed"));
         return 0;
      }
      if (strchr(str, ' ')) {
         show_error(loc("Password may not contain blanks"));
         return 0;
      }
   }

   /* check self register flag */
   self_register = 0;
   if (getcfg(lbs->name, "Self register", str, sizeof(str)))
      self_register = atoi(str);

   /* check if passwords match */
   new_pwd[0] = 0;
   if (isparam("newpwd") && isparam("newpwd2")) {
      strlcpy(new_pwd, getparam("newpwd"), sizeof(new_pwd));
      strlcpy(new_pwd2, getparam("newpwd2"), sizeof(new_pwd2));

      if (strcmp(new_pwd, new_pwd2) != 0) {
         show_error(loc("New passwords do not match, please retype"));
         return 0;
      }
   }

   /* check if first user */
   first_user = !enum_user_line(lbs, 0, str, sizeof(str));

   /* check if user exists */
   if (new_user) {
      if (get_user_line(lbs, user_enc, NULL, NULL, NULL, NULL, NULL, NULL) == 1) {
         sprintf(str, "%s \"%s\" %s", loc("Login name"), user_enc, loc("exists already"));
         show_error(str);
         return 0;
      }
   }

   /* if register through selection page, use first logbook with same password file */
   if (lbs == NULL) {
      getcfg(NULL, "password file", file_name, sizeof(file_name));
      for (i = 0; lb_list[i].name[0]; i++) {
         getcfg(lb_list[i].name, "password file", str, sizeof(str));
         if (strieq(file_name, str))
            break;
      }
      if (lb_list[i].name[0] == 0)
         lbs = &lb_list[0];
      else
         lbs = &lb_list[i];
   }

   getcfg(lbs->name, "Password file", str, sizeof(str));

   if (lbs->pwd_xml_tree == NULL)
      return 0;

   sprintf(str, "/list/user[name=%s]", user_enc);
   node = mxml_find_node(lbs->pwd_xml_tree, str);

   code = 0;

   if (new_user) {
      node = mxml_find_node(lbs->pwd_xml_tree, "/list");
      if (!node) {
         show_error(loc("Error accessing password file"));
         return 0;
      }
      node = mxml_add_node(node, "user", NULL);

      if (isparam("new_user_name")) {
         strencode2(str, getparam("new_user_name"), sizeof(str));
         mxml_add_node(node, "name", str);
      }
#ifdef HAVE_PAM
                                                                                                                              getcfg(lbs->name, "Authentication", str, sizeof(str));
      if (!stristr(str, "PAM")) {
#endif /* HAVE_PAM */
      do_crypt(new_pwd, str, sizeof(str));
      npwd = mxml_add_node(node, "password", str);
      if (npwd)
         mxml_add_attribute(npwd, "encoding", "SHA256");
#ifdef HAVE_PAM
      }
#endif /* HAVE_PAM */

      if (isparam("new_full_name")) {
         strencode2(str, getparam("new_full_name"), sizeof(str));
         mxml_add_node(node, "full_name", str);
      }
      mxml_add_node(node, "last_logout", "0");
      mxml_add_node(node, "last_activity", "0");
      if (isparam("new_user_email"))
         mxml_add_node(node, "email", getparam("new_user_email"));

      if (!first_user && (self_register == 3 || self_register == 4)) {
         code = rand() + (rand() << 16);
         sprintf(str, "%d", code);
         mxml_add_node(node, "inactive", str);
      } else
         mxml_add_node(node, "inactive", "0");

   } else {
      /* replace record */
      if (isparam("new_user_name")) {
         strencode2(str, getparam("new_user_name"), sizeof(str));
         mxml_replace_subvalue(node, "name", str);
      }
      if (new_pwd[0])
         mxml_replace_subvalue(node, "password", new_pwd);
      if (isparam("new_full_name")) {
         strencode2(str, getparam("new_full_name"), sizeof(str));
         mxml_replace_subvalue(node, "full_name", str);
      }
      if (isparam("new_user_email"))
         mxml_replace_subvalue(node, "email", getparam("new_user_email"));
      if (isparam("active"))
         mxml_replace_subvalue(node, "inactive", "0");
      else
         mxml_replace_subvalue(node, "inactive", "1");
   }

   subnode = mxml_find_node(node, "email_notify");
   if (subnode)
      mxml_delete_node(subnode);
   mxml_add_node(node, "email_notify", NULL);
   subnode = mxml_find_node(node, "email_notify");
   for (i = 0; lb_list[i].name[0]; i++) {
      sprintf(str, "sub_lb%d", i);
      if (isparam(str) && getparam(str) && atoi(getparam(str)))
         mxml_add_node(subnode, "logbook", lb_list[i].name);
   }

   if (get_password_file(lbs, file_name, sizeof(file_name)))
      mxml_write_tree(file_name, lbs->pwd_xml_tree);

   /* if requested, send notification email to user or admin user */
   if (new_user && !first_user && (self_register == 2 || self_register == 3 || self_register == 4)
       && !isparam("admin")) {
      if (!getcfg("global", "SMTP host", smtp_host, sizeof(smtp_host))) {
         show_error(loc("No SMTP host defined in [global] section of configuration file"));
         return 0;
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

      /* send email to new user */
      if (self_register == 4) {
         if (lbs)
            sprintf(subject, loc("Account activation for ELOG logbook \"%s\""), lbs->name);
         else
            sprintf(subject, loc("Account activation for ELOG on host \"%s\""), host_name);

         if (!isparam("new_user_email")) {
            show_error("A valid email address necessary for this ELOG account");
            return 0;
         }

         strlcpy(email_addr, getparam("new_user_email"), sizeof(email_addr));

         mail_text[0] = 0;
         compose_email_header(lbs, subject, mail_from_name, email_addr,
                              NULL, mail_text, sizeof(mail_text), 1, 0, NULL, 0, 0);
         sprintf(mail_text + strlen(mail_text), "\r\n%s:\r\n\r\n",
                 loc("Please click the URL below to activate following ELOG account"));

         if (lbs)
            sprintf(mail_text + strlen(mail_text), "%s             : %s\r\n", loc("Logbook"), lbs->name);
         else
            sprintf(mail_text + strlen(mail_text), "%s                : %s\r\n", loc("Host"), host_name);

         if (isparam("new_user_name"))
            sprintf(mail_text + strlen(mail_text), "%s          : %s\r\n", loc("Login name"),
                    getparam("new_user_name"));
         if (isparam("new_full_name"))
            sprintf(mail_text + strlen(mail_text), "%s           : %s\r\n", loc("Full name"),
                    getparam("new_full_name"));
         if (isparam("new_user_email"))
            sprintf(mail_text + strlen(mail_text), "%s               : %s\r\n", loc("Email"),
                    getparam("new_user_email"));

         sprintf(mail_text + strlen(mail_text), "\r\n%s:\r\n", loc("Activation URL"));

         sprintf(mail_text + strlen(mail_text), "\r\nURL                 : %s", url);

         if (isparam("new_user_name"))
            sprintf(mail_text + strlen(mail_text), "?cmd=%s", loc("Activate"));

         sprintf(mail_text + strlen(mail_text), "&code=%d&unm=%s\r\n", code, getparam("new_user_name"));

         if (sendmail(lbs, smtp_host, mail_from, email_addr, mail_text, error, sizeof(error)) == -1) {
            sprintf(str, loc("Cannot send email notification to \"%s\""), getparam("new_user_email"));
            strlcat(str, " : ", sizeof(str));
            strlcat(str, error, sizeof(str));
            strencode2(str2, str, sizeof(str2));
            show_error(str2);
            return 0;
         };

         show_standard_header(lbs, FALSE, loc("ELOG registration"), "", FALSE, NULL, NULL, 0);
         rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");
         rsprintf("<tr><td colspan=2 class=\"dlgtitle\">\n");
         rsprintf(loc("An email has been sent to &lt;%s&gt;"), getparam("new_user_email"));
         rsprintf(".<br>\n");
         rsprintf(loc("Use that email to activate your account"));
         rsprintf(".</td></tr></table>\n");
         show_bottom_text(lbs);
         rsprintf("</body></html>\n");
         return 0;
      }

      /* send email to admin user(s) */
      if (self_register == 2 || self_register == 3) {
         if (getcfg(lbs->name, "Admin user", admin_user, sizeof(admin_user))) {
            pl = strtok(admin_user, " ,");
            while (pl) {
               get_user_line(lbs, pl, NULL, NULL, email_addr, NULL, NULL, NULL);
               if (email_addr[0]) {

                  /* compose subject */
                  if (self_register == 3) {
                     if (lbs)
                        sprintf(subject, loc("Registration request for ELOG logbook \"%s\""), lbs->name);
                     else
                        sprintf(subject, loc("Registration request for ELOG on host \"%s\""), host_name);
                     sprintf(str, loc("A new ELOG user wants to register on \"%s\""), host_name);
                  } else {
                     if (isparam("new_user_name")) {
                        if (lbs)
                           sprintf(subject, loc("User \"%s\" registered on logbook \"%s\""),
                                   getparam("new_user_name"), lbs->name);
                        else
                           sprintf(subject, loc("User \"%s\" registered on host \"%s\""),
                                   getparam("new_user_name"), host_name);
                     }

                     sprintf(str, loc("A new ELOG user has been registered on %s"), host_name);
                  }

                  mail_text[0] = 0;
                  compose_email_header(lbs, subject, mail_from_name, email_addr,
                                       NULL, mail_text, sizeof(mail_text), 1, 0, NULL, 0, 0);
                  sprintf(mail_text + strlen(mail_text), "\r\n%s:\r\n\r\n", str);

                  if (lbs)
                     sprintf(mail_text + strlen(mail_text), "%s             : %s\r\n", loc("Logbook"),
                             lbs->name);
                  else
                     sprintf(mail_text + strlen(mail_text), "%s                : %s\r\n", loc("Host"),
                             host_name);

                  if (isparam("new_user_name"))
                     sprintf(mail_text + strlen(mail_text), "%s          : %s\r\n", loc("Login name"),
                             getparam("new_user_name"));
                  if (isparam("new_full_name"))
                     sprintf(mail_text + strlen(mail_text), "%s           : %s\r\n", loc("Full name"),
                             getparam("new_full_name"));
                  if (isparam("new_user_email"))
                     sprintf(mail_text + strlen(mail_text), "%s               : %s\r\n", loc("Email"),
                             getparam("new_user_email"));

                  if (self_register == 3) {
                     sprintf(mail_text + strlen(mail_text), "\r\n%s:\r\n",
                             loc("Hit following URL to activate that account"));

                     sprintf(mail_text + strlen(mail_text), "\r\nURL                 : %s", url);

                     if (isparam("new_user_name"))
                        sprintf(mail_text + strlen(mail_text), "?cmd=%s&new_user_name=%s",
                                loc("Activate"), getparam("new_user_name"));

                     sprintf(mail_text + strlen(mail_text), "&code=%d&unm=%s\r\n", code, pl);
                  } else {
                     if (isparam("new_user_name"))
                        sprintf(mail_text + strlen(mail_text),
                                "\r\n%s URL         : %s?cmd=Config&cfg_user=%s&unm=%s\r\n", loc("Logbook"),
                                url, getparam("new_user_name"), pl);
                  }

                  if (sendmail(lbs, smtp_host, mail_from, email_addr, mail_text, error, sizeof(error)) == -1) {
                     sprintf(str, loc("Cannot send email notification to \"%s\""),
                             getparam("new_user_email"));
                     strlcat(str, " : ", sizeof(str));
                     strlcat(str, error, sizeof(str));
                     strencode2(str2, str, sizeof(str2));
                     show_error(str2);
                     return 0;
                  };
               }

               pl = strtok(NULL, " ,");
            }
         } else {
            show_error(loc("No admin user has been defined in configuration file"));
            return 0;
         }

         if (self_register == 3) {
            sprintf(str, "?cmd=%s", loc("Requested"));
            redirect(lbs, str);
            return 0;
         }
      }
   }

   /* log in user automatically */
   if (new_user && (self_register == 1 || self_register == 2)) {

      if (isparam("new_user_name")) {
         /* get a new session ID */
         sid_new(lbs, getparam("new_user_name"), (char *) inet_ntoa(rem_addr), sid);

         if (lbs)
            snprintf(str, sizeof(str), "../%s/", lbs->name_enc);
         else
            sprintf(str, ".");
         if (isparam("new_user_name")) {
            sprintf(str + strlen(str), "?cmd=%s&cfg_user=", loc("Config"));
            strlcat(str, getparam("new_user_name"), sizeof(str));
         } else if (isparam("cfg_user")) {
            sprintf(str + strlen(str), "?cmd=%s&cfg_user=", loc("Config"));
            strlcat(str, getparam("cfg_user"), sizeof(str));
         } else if (getcfg(lbs->name, "password file", str2, sizeof(str2)))
            sprintf(str + strlen(str), "?cmd=%s", loc("Config"));
         setparam("redir", str);

         /* set SID cookie */
         set_sid_cookie(lbs, sid, getparam("new_user_name"));

         return 0;
      }
   }

   return 1;
}
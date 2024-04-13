void show_config_page(LOGBOOK *lbs) {
   char str[256], user[80], password[80], full_name[256], user_email[256], logbook[256], auth[32], **user_list;
   int i, j, n, cols, inactive;
   BOOL email_notify[1000], sort_email;

   if (lbs)
      strcpy(logbook, lbs->name);
   else
      strcpy(logbook, "global");

   /* get user */
   strcpy(user, isparam("unm") ? getparam("unm") : "");
   if (isparam("cfg_user"))
      strcpy(user, getparam("cfg_user"));

   /* get sort_email flag */
   sort_email = FALSE;
   if (isparam("sort_email") && atoi(getparam("sort_email")) > 0)
      sort_email = TRUE;

   /*---- header ----*/

   show_standard_header(lbs, TRUE, loc("ELOG user config"), ".", FALSE, NULL, NULL, 0);

   /*---- javascript to warn removal of user and of deactivation ----*/
   rsprintf("<script type=\"text/javascript\">\n");
   rsprintf("<!--\n\n");
   rsprintf("function chkrem()\n");
   rsprintf("{\n");
   strencode2(str, user, sizeof(str));
   strlcpy(user, str, sizeof(user));
   sprintf(str, loc("Really remove user \\\"%s\\\"?"), user);
   rsprintf("    var subm = confirm(\"%s\");\n", str);
   rsprintf("    return subm;\n");
   rsprintf("}\n\n");
   rsprintf("function chkdeact(c)\n");
   rsprintf("{\n");
   strlcpy(str, loc("Are you sure you want to deactivate your own account?"), sizeof(str));
   rsprintf("    if (c.checked == false)\n");
   rsprintf("       return confirm(\"%s\");\n", str);
   rsprintf("    return true;\n");
   rsprintf("}\n\n");
   rsprintf("//-->\n");
   rsprintf("</script>\n\n");

   /*---- title ----*/

   show_standard_title(lbs, "", 0);

   /*---- activation notice ----*/

   if (isparam("notice")) {
      rsprintf("<tr><td class=\"notifymsg\" colspan=2>\n");
      rsprintf(getparam("notice"));
      rsprintf("</tr>\n");
   }

   /*---- menu buttons ----*/

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

   rsprintf("<input type=hidden name=cmd value=\"%s\">\n", loc("Config"));      // for select javascript
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Save"));
   rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Back"));
   strencode2(str, user, sizeof(str));
   rsprintf("<input type=hidden name=config value=\"%s\">\n", str);
   rsprintf("<input type=hidden name=cfgpage value=\"1\">\n");                  // needed for "Save" command

   rsprintf("</span></td></tr>\n\n");

   /* table for two-column items */
   rsprintf("<tr><td class=\"form2\">");
   rsprintf("<table width=\"100%%\" cellspacing=0>\n");

   /*---- if admin user, show user list ----*/

   if (is_admin_user(lbs, getparam("unm"))) {
      rsprintf("<input type=hidden name=admin value=1>\n");
      rsprintf("<tr><td nowrap width=\"10%%\">%s:</td>\n", loc("Select user"));
      rsprintf("<td><select name=cfg_user onChange=\"document.form1.submit()\">\n");

      /* count user list */
      for (n = 0;; n++) {
         if (!enum_user_line(lbs, n, str, sizeof(str)))
            break;
      }

      /* allocate list of users and populate it */
      user_list = (char **) xcalloc(sizeof(char *), n);
      for (i = 0; i < n; i++)
         user_list[i] = (char *) xcalloc(NAME_LENGTH, 1);

      for (i = 0; i < n; i++) {
         enum_user_line(lbs, i, user_list[i], NAME_LENGTH);
         get_user_line(lbs, user_list[i], NULL, full_name, user_email, NULL, NULL, NULL);
         if (sort_email)
            strlcpy(user_list[i], user_email, NAME_LENGTH);
      }

      /* sort list */
      qsort(user_list, n, sizeof(char *), ascii_compare);

      for (i = 0; i < n; i++) {
         if (sort_email) {
            strlcpy(user_email, user_list[i], sizeof(user_email));
            user_list[i][0] = 0;
            get_user_line(lbs, user_list[i], NULL, full_name, user_email, NULL, NULL, NULL);
         } else
            get_user_line(lbs, user_list[i], NULL, full_name, user_email, NULL, NULL, NULL);
         if (strcmp(user_list[i], user) == 0) {
            strencode2(str, user_list[i], sizeof(str));
            if (sort_email)
               rsprintf("<option selected value=\"%s\">&lt;%s&gt; %s\n", str, user_email, str);
            else
               rsprintf("<option selected value=\"%s\">%s &lt;%s&gt;\n", str, str, user_email);
         } else {
            strencode2(str, user_list[i], sizeof(str));
            if (sort_email)
               rsprintf("<option value=\"%s\">&lt;%s&gt; %s\n", str, user_email, str);
            else
               rsprintf("<option value=\"%s\">%s &lt;%s&gt;\n", str, str, user_email);

         }
      }

      for (i = 0; i < n; i++)
         xfree(user_list[i]);
      xfree(user_list);

      rsprintf("</select>\n");

      /* show "update" button only of javascript is not enabled */
      rsprintf("<noscript>\n");
      rsprintf("<input type=submit value=\"%s\">\n", loc("Update"));
      rsprintf("</noscript>\n");

      if (sort_email)
         rsprintf(
                 "<input type=\"checkbox\" checked name=\"sort_email\" value=\"1\" onChange=\"document.form1.submit()\">Sort by email");
      else
         rsprintf(
                 "<input type=\"checkbox\" name=\"sort_email\" value=\"1\" onChange=\"document.form1.submit()\">Sort by email");
   }

   /*---- entry form ----*/

   if (get_user_line(lbs, user, password, full_name, user_email, email_notify, NULL, &inactive) != 1)
      sprintf(str, loc("User [%s] has been deleted"), user);
   else
      strlcpy(str, user, sizeof(str));

   if (is_admin_user(lbs, getparam("unm"))) {
      rsprintf("<tr><td nowrap width=\"15%%\">%s:</td>\n", loc("Active"));
      if (stricmp(user, getparam("unm")) == 0)
         rsprintf
                 ("<td><input type=checkbox name=active value=1 %s onClick=\"return chkdeact(this);\"></td></tr>\n",
                  inactive ? "" : "checked");
      else
         rsprintf("<td><input type=checkbox name=active value=1 %s></td></tr>\n", inactive ? "" : "checked");
   } else {
      rsprintf("<tr><td nowrap width=\"10%%\">%s:</td>\n", loc("Active"));
      rsprintf("<td><input type=checkbox name=active value=1 readonly %s></td></tr>\n",
               inactive ? "" : "checked");
   }

   rsprintf("<tr><td nowrap width=\"15%%\">%s:</td>\n", loc("Login name"));

   getcfg(lbs->name, "Authentication", auth, sizeof(auth));

   strencode2(str, user, sizeof(str));
   if (stristr(auth, "Kerberos") || stristr(auth, "Webserver") || stristr(auth, "PAM"))
      rsprintf("<td><input type=text size=40 name=new_user_name value=\"%s\" readonly></td></tr>\n", str);
   else
      rsprintf("<td><input type=text size=40 name=new_user_name value=\"%s\"></td></tr>\n", str);

   rsprintf("<tr><td nowrap width=\"15%%\">%s:</td>\n", loc("Full name"));
   rsprintf("<td><input type=text size=40 name=new_full_name value=\"%s\"></tr>\n", full_name);

   rsprintf("<tr><td nowrap width=\"15%%\">Email:</td>\n");
   rsprintf("<td><input type=text size=40 name=new_user_email value=\"%s\"></td></tr>\n", user_email);

   for (i = n = 0; lb_list[i].name[0]; i++) {

      if (!getcfg_topgroup() || strieq(getcfg_topgroup(), lb_list[i].top_group)) {

         /* check if user has access */
         if (!isparam("unm") || check_login_user(&lb_list[i], getparam("unm"))) {

            /* check if emails are enabled for this logbook */
            if (!getcfg(lb_list[i].name, "Suppress email to users", str, sizeof(str)) || atoi(str) == 0)
               n++;
         }
      }
   }

   if (n > 0) {
      for (i = 0; lb_list[i].name[0]; i++) {}
      j = (int) (i / 16) + 1;
      cols = ((j > 5) ? 5 : j);
      rsprintf("<tr><td colspan=\"2\"><br />%s:\n", loc("Subscribe to logbooks"));

      rsprintf("<br><span class=\"selcomment\"><b>(%s)</b></span>\n",
               loc("enable automatic email notifications"));

      rsprintf("<td></tr>\n");

      rsprintf("<tr><td colspan=\"2\"><table><tr>\n");

      for (j = i = 0; lb_list[i].name[0]; i++) {

         if (!getcfg_topgroup() || strieq(getcfg_topgroup(), lb_list[i].top_group)) {

            /* check if user has access */
            if (!isparam("unm") || check_login_user(&lb_list[i], getparam("unm"))) {

               /* check if emails are enabled for this logbook */
               if (!getcfg(lb_list[i].name, "Suppress email to users", str, sizeof(str)) || atoi(str) == 0) {
                  if (email_notify[i])
                     rsprintf("<td><input type=checkbox checked id=\"lb%d\" name=\"sub_lb%d\" value=\"1\">\n",
                              i,
                              i);
                  else
                     rsprintf("<td><input type=checkbox id=\"lb%d\" name=\"sub_lb%d\" value=\"1\">\n", i, i);
                  rsprintf("<label for=\"lb%d\">%s</label></td>\n", i, lb_list[i].name);
                  j++;
               }
            }
         }
         if (j > 0 && (j % cols) == 0)
            rsprintf("</tr>\n<tr>");
      }
      rsprintf("</tr></table><br />\n");
   }

   if (n > 2) {
      rsprintf("<script language=\"JavaScript\" type=\"text/javascript\">\n");
      rsprintf("<!--\n");
      rsprintf("function SetNone()\n");
      rsprintf("  {\n");
      rsprintf("    for (var i=0,els=document.querySelectorAll('[name^=\"sub_lb\"]') ; i<els.length ; i++)\n");
      rsprintf("      els[i].checked = false;\n");
      rsprintf("  }\n");
      rsprintf("function SetAll()\n");
      rsprintf("  {\n");
      rsprintf("    for (var i=0,els=document.querySelectorAll('[name^=\"sub_lb\"]') ; i<els.length ; i++)\n");
      rsprintf("      els[i].checked = true;\n");
      rsprintf("  }\n");
      rsprintf("//-->\n");
      rsprintf("</script>\n");

      rsprintf("<input type=button value=\"%s\" onClick=\"SetAll();\">\n", loc("Set all"));
      rsprintf("<input type=button value=\"%s\" onClick=\"SetNone();\">\n", loc("Set none"));
   }

   rsprintf("</td></tr>\n");

   rsprintf("</table></td></tr>\n");

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu1\">\n");

   if (is_admin_user(lbs, getparam("unm")) || !getcfg(logbook, "allow password change", str, sizeof(str))
       || atoi(str) == 1)
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("Change password"));

   rsprintf("<input type=submit name=cmd value=\"%s\" onClick=\"return chkrem();\">\n", loc("Remove user"));

   if (is_admin_user(lbs, getparam("unm"))) {
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", loc("New user"));
      strlcpy(str, loc("Change config file"), sizeof(str));
      rsprintf("<input type=submit name=cmd value=\"%s\">\n", str);
   }

   rsprintf("</span></td></tr></table>\n\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");
}
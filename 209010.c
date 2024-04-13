void show_change_pwd_page(LOGBOOK *lbs) {
   char str[256], config[256], old_pwd[256], new_pwd[256], new_pwd2[256], user[256], auth[32], error_str[256];
   int wrong_pwd;
   /* otherwise calls with null lbs which make this procedure crash */
   if (lbs == NULL)
      lbs = get_first_lbs_with_global_passwd();

   if (lbs == NULL)
      return;
   getcfg(lbs->name, "Authentication", auth, sizeof(auth));

   old_pwd[0] = new_pwd[0] = new_pwd2[0] = 0;

   if (isparam("oldpwd"))
      strlcpy(old_pwd, getparam("oldpwd"), sizeof(old_pwd));
   if (isparam("newpwd"))
      strlcpy(new_pwd, getparam("newpwd"), sizeof(new_pwd));
   if (isparam("newpwd2"))
      strlcpy(new_pwd2, getparam("newpwd2"), sizeof(new_pwd2));

   if (isparam("unm"))
      strlcpy(user, getparam("unm"), sizeof(user));
   else
      user[0] = 0;

   if (isparam("config")) {
      strlcpy(str, getparam("config"), sizeof(str));
      strencode2(user, str, sizeof(user));
   }

   wrong_pwd = FALSE;
   error_str[0] = 0;

   if (isparam("fail")) {
      wrong_pwd = atoi(getparam("fail"));
      if (!wrong_pwd) {
         wrong_pwd = 3;
         strlcpy(error_str, getparam("fail"), sizeof(error_str));
      }
   }

   if (old_pwd[0] || new_pwd[0]) {
      if (user[0]) {

         if (stristr(auth, "Kerberos") || stristr(auth, "Webserver") || stristr(auth, "PAM")) {
            if (strcmp(new_pwd, new_pwd2) != 0)
               wrong_pwd = 2;
         } else {
            /* administrator does not have to supply old password if changing other user's password */
            if (isparam("unm") && is_admin_user(lbs, getparam("unm"))
                && stricmp(getparam("unm"), user) != 0)
               wrong_pwd = 0;
            else {
               if (!auth_verify_password(lbs, user, old_pwd, str, sizeof(str)))
                  wrong_pwd = 1;
            }

            if (strcmp(new_pwd, new_pwd2) != 0)
               wrong_pwd = 2;
         }
      }

      if (new_pwd[0]) {
         /* replace password */
         error_str[0] = 0;
         if (!wrong_pwd)
            wrong_pwd =
                    (auth_change_password(lbs, user, old_pwd, new_pwd, error_str, sizeof(error_str)) == 0);

         if (!wrong_pwd && isparam("unm") && strcmp(user, getparam("unm")) == 0) {
            redirect(lbs, "");
            return;
         }

         if (!wrong_pwd) {
            /* redirect back to configuration page */
            if (isparam("config")) {
               strlcpy(config, getparam("config"), sizeof(config));
               sprintf(str, "?cmd=%s&cfg_user=%s", loc("Config"), config);
            } else
               sprintf(str, "?cmd=%s", loc("Config"));
            redirect(lbs, str);
            return;
         }

         /* go though one redirection to avoid passwords to be shown in URL */
         if (wrong_pwd) {
            if (error_str[0])
               sprintf(str, "?cmd=%s&config=%s&fail=%s", loc("Change password"), getparam("unm"), error_str);
            else
               sprintf(str, "?cmd=%s&config=%s&fail=%d", loc("Change password"), getparam("unm"), wrong_pwd);
            redirect(lbs, str);
            return;
         }
      }
   }

   show_standard_header(lbs, TRUE, loc("ELOG change password"), NULL, FALSE, NULL, NULL, 0);

   rsprintf("<table class=\"dlgframe\" cellspacing=0 align=center>");

   if (wrong_pwd == 1) {
      if (error_str[0]) {
         strencode2(str, error_str, sizeof(str));
         rsprintf("<tr><td colspan=2 class=\"dlgerror\">%s!</td></tr>\n", str);
      } else
         rsprintf("<tr><td colspan=2 class=\"dlgerror\">%s!</td></tr>\n", loc("Wrong password"));
   }

   if (wrong_pwd == 2)
      rsprintf("<tr><td colspan=2 class=\"dlgerror\">%s!</td></tr>\n",
               loc("New passwords do not match, please retype"));

   if (wrong_pwd == 3) {
      strencode2(str, error_str, sizeof(str));
      rsprintf("<tr><td colspan=2 class=\"dlgerror\">%s!</td></tr>\n", str);
   }

   rsprintf("<tr><td colspan=2 class=\"dlgtitle\">\n");

   rsprintf("<input type=hidden name=config value=\"%s\">", user);

   rsprintf("%s \"%s\"</td></tr>\n", loc("Change password for user"), user);

   /* do not ask for old pwasword if admin changes other user's password */
   if (isparam("unm")) {
      if (!is_admin_user(lbs, getparam("unm")) || stricmp(getparam("unm"), user) == 0) {
         if (isparam("oldpwd") && !(wrong_pwd == 1))    // hidden password for password recovery
            rsprintf("<input type=hidden name=oldpwd value=\"%s\"", getparam("oldpwd"));
         else {
            rsprintf("<tr><td align=right class=\"dlgform\">%s:\n", loc("Old password"));
            rsprintf("<td align=left class=\"dlgform\"><input type=password name=oldpwd>\n");
            rsprintf("</td></tr>\n");
         }
      }
   }

   rsprintf("<tr><td align=right class=\"dlgform\">%s:</td>\n", loc("New password"));
   rsprintf("<td align=left class=\"dlgform\"><input type=password name=newpwd></td></tr>\n");

   rsprintf("<tr><td align=right class=\"dlgform\">%s:</td>\n", loc("Retype new password"));
   rsprintf("<td align=left class=\"dlgform\"><input type=password name=newpwd2></td></tr>\n");

   rsprintf("<tr><td align=center colspan=2 class=\"dlgform\"><input type=submit value=\"%s\"></td></tr>",
            loc("Submit"));

   rsprintf("</table>\n");
   show_bottom_text(lbs);
   rsprintf("</form></body></html>\r\n");
}
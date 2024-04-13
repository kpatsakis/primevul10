void set_sid_cookie(LOGBOOK *lbs, char *sid, char *full_name) {
   char str[256], lb_name[256], exp[80];
   BOOL global;

   rsprintf("HTTP/1.1 302 Found\r\n");
   rsprintf("Server: ELOG HTTP %s-%s\r\n", VERSION, git_revision());
   if (keep_alive) {
      rsprintf("Connection: Keep-Alive\r\n");
      rsprintf("Keep-Alive: timeout=60, max=10\r\n");
   }

   if (lbs)
      strcpy(lb_name, lbs->name);
   else
      strcpy(lb_name, "global");

   /* get optional expriation from configuration file */
   if (getcfg(lb_name, "Login expiration", str, sizeof(str)) || atof(str) > 0)
      strcpy(exp, str);
   else if (isparam("remember")) {
      strcpy(exp, "744");       /* one month by default = 31*24 */
   } else
      exp[0] = 0;

   /* check if cookies should be global */
   global = getcfg("global", "Password file", str, sizeof(str));

   /* set the session ID cookie */
   set_cookie(lbs, "sid", sid, global, exp);

   /* set the use full name cookie */
   set_cookie(lbs, "ufnm", full_name, global, exp);

   /* set "remember me" cookie on login */
   if (isparam("remember"))
      set_cookie(lbs, "urem", "1", global, "8760");     /* one year = 24*365 */
   else
      set_cookie(lbs, "urem", "0", global, "8760");

   set_redir(lbs, isparam("redir") ? getparam("redir") : "");
}
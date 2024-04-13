BOOL logged_in(LOGBOOK *lbs) {
   if (isparam("unm")) {
      if (check_login_user(lbs, getparam("unm")) && check_login(lbs, getparam("sid")))
         return TRUE;
   }
   return FALSE;
}
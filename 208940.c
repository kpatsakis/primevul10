int html_allowed(LOGBOOK *lbs) {
   char str[80];

   return (getcfg(lbs->name, "Allow HTML", str, sizeof(str)) && atoi(str) == 1);
}
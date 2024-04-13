int seteuser(char *str) {
#ifdef OS_UNIX
   struct passwd *pw;

   pw = getpwnam(str);

   if (pw != NULL) {
      chown(logbook_dir, pw->pw_uid, -1);
      if (setreuid(-1, pw->pw_uid) >= 0) {
         return 0;
      } else {
         eprintf("Cannot set effective UID to user \"%s\"\n", str);
         eprintf("setuser: %s\n", strerror(errno));
      }
   } else
      eprintf("User \"%s\" not found\n", str);

   return -1;
#else
   return 0;
#endif
}
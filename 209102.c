int setgroup(char *str) {
#ifdef OS_UNIX
   struct group *gr;

   gr = getgrnam(str);

   if (gr != NULL)
      if (setgid(gr->gr_gid) >= 0 && initgroups(gr->gr_name, gr->gr_gid) >= 0)
         return 0;
      else {
         eprintf("Cannot set effective GID to group \"%s\"\n", gr->gr_name);
         eprintf("setgroup: %s\n", strerror(errno));
      }
   else
      eprintf("Group \"%s\" not found\n", str);

   return -1;
#else
   return 0;
#endif
}
int ss_daemon_init() {
#ifdef OS_UNIX

   /* only implemented for UNIX */
   int i, fd, pid;

   if ((pid = fork()) < 0)
      return FAILURE;
   else if (pid != 0)
      _exit(EXIT_SUCCESS);      /* parent finished, exit without atexit hook */

   /* child continues here */

   /* try and use up stdin, stdout and stderr, so other
      routines writing to stdout etc won't cause havoc. Copied from smbd */
   for (i = 0; i < 3; i++) {
      close(i);
      fd = open("/dev/null", O_RDWR, 0);
      if (fd < 0)
         fd = open("/dev/null", O_WRONLY, 0);
      if (fd < 0) {
         eprintf("Can't open /dev/null");
         return FAILURE;
      }
      if (fd != i) {
         eprintf("Did not get file descriptor");
         return FAILURE;
      }
   }

   setsid();                    /* become session leader */
   chdir("/");                  /* change working direcotry (not on NFS!) */
   umask(0);                    /* clear our file mode creation mask */

#endif

   return SUCCESS;
}
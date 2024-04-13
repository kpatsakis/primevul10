dconnect ()
{
  struct sockaddr_un userver;
  int asockd;


  memset ((char *) &userver, 0, sizeof (userver));

  ci_debug_printf(1, "dconnect: entering.\n");
  if (clamd_local != NULL) {
      userver.sun_family = AF_UNIX;
      xstrncpy (userver.sun_path, clamd_local, sizeof(userver.sun_path));
      if ((asockd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
          ci_debug_printf(0, "ERROR dconnect: Can't bind local socket on %s.\n", clamd_local);
          return -1;
      }
      if (connect (asockd, (struct sockaddr *) &userver, sizeof (struct sockaddr_un)) < 0) {
          close (asockd);
          ci_debug_printf(0, "ERROR dconnect: Can't connect to clamd on local socket %s.\n", clamd_local);
          return -1;
      }
      return asockd;

    } else {
        if (clamd_curr_ip[0] != 0) {
                asockd = connectINET(clamd_curr_ip, atoi(clamd_port));
                if ( asockd != -1 ) {
                   ci_debug_printf(1, "DEBUG dconnect: Connected to Clamd (%s:%s)\n", clamd_curr_ip,clamd_port);
                    return asockd;
                }
        }

        char *ptr;
        char *s = (char *) malloc (sizeof (char) * SMALL_CHAR);
        xstrncpy(s, clamd_ip, SMALL_CHAR);
        ptr = strtok(s, ",");
        while (ptr != NULL) {
                asockd = connectINET(ptr, atoi(clamd_port));
                if ( asockd != -1 ) {
                    ci_debug_printf(1, "DEBUG dconnect: Connected to Clamd (%s:%s)\n", ptr,clamd_port);
                    /* Store last working clamd */
                    xstrncpy(clamd_curr_ip, ptr, LOW_CHAR);
                    xfree(s);
                    break;
                }
                ptr = strtok(NULL, ",");
        }
        return asockd;
        xfree(s);
    }
    return 0;
}
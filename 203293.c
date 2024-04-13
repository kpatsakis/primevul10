main(int argc, char **argv)
{
  char *line;
  const char *progname = argv[0];
  const char *hostname = "localhost";
  const char *conf_file = DEFAULT_CONF_FILE;
  int quit = 0, ret = 1, multi = 0, auto_auth = 0;
  int port = DEFAULT_CANDM_PORT;

  /* Parse command line options */
  while (++argv, --argc) {
    if (!strcmp(*argv, "-h")) {
      ++argv, --argc;
      if (*argv) {
        hostname = *argv;
      }
    } else if (!strcmp(*argv, "-p")) {
      ++argv, --argc;
      if (*argv) {
        port = atoi(*argv);
      }
    } else if (!strcmp(*argv, "-f")) {
      ++argv, --argc;
      if (*argv) {
        conf_file = *argv;
      }
    } else if (!strcmp(*argv, "-a")) {
      auto_auth = 1;
    } else if (!strcmp(*argv, "-m")) {
      multi = 1;
    } else if (!strcmp(*argv, "-n")) {
      no_dns = 1;
    } else if (!strcmp(*argv, "-4")) {
      DNS_SetAddressFamily(IPADDR_INET4);
      hostname = "127.0.0.1";
    } else if (!strcmp(*argv, "-6")) {
      DNS_SetAddressFamily(IPADDR_INET6);
      hostname = "::1";
    } else if (!strcmp("-v", *argv) || !strcmp("--version",*argv)) {
      printf("chronyc (chrony) version %s\n", CHRONY_VERSION);
      exit(0);
    } else if (!strncmp(*argv, "-", 1)) {
      fprintf(stderr, "Usage : %s [-h <hostname>] [-p <port-number>] [-n] [-4|-6] [-m] [-a] [-f <file>]] [command]\n", progname);
      exit(1);
    } else {
      break; /* And process remainder of line as a command */
    }
  }

  if (isatty(0) && isatty(1) && isatty(2)) {
    on_terminal = 1;
  }

  if (on_terminal && (argc == 0)) {
    display_gpl();
  }

  /* MD5 is the default authentication hash */
  auth_hash_id = HSH_GetHashId("MD5");
  if (auth_hash_id < 0) {
    fprintf(stderr, "Could not initialize MD5\n");
    return 1;
  }
  
  open_io(hostname, port);

  if (auto_auth) {
    ret = authenticate_from_config(conf_file);
  }

  if (!ret) {
    ;
  } else if (argc > 0) {
    ret = process_args(argc, argv, multi);
  } else {
    do {
      line = read_line();
      if (line) {
        ret = process_line(line, &quit);
      }else {
	/* supply the final '\n' when user exits via ^D */
        if( on_terminal ) printf("\n");
      }
    } while (line && !quit);
  }

  close_io();

  free(password);

  return !ret;
}
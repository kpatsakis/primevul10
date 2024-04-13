CAM_Initialise(int family)
{
  assert(!initialised);
  initialised = 1;

  assert(sizeof (permissions) / sizeof (permissions[0]) == N_REQUEST_TYPES);

  utoken = (unsigned long) time(NULL);

  issued_tokens = returned_tokens = issue_pointer = 0;
  token_base = 1; /* zero is the value used when the previous command was
                     unauthenticated */

  free_replies = NULL;
  kept_replies.next = NULL;

  if (family == IPADDR_UNSPEC || family == IPADDR_INET4)
    sock_fd4 = prepare_socket(AF_INET);
  else
    sock_fd4 = -1;
#ifdef HAVE_IPV6
  if (family == IPADDR_UNSPEC || family == IPADDR_INET6)
    sock_fd6 = prepare_socket(AF_INET6);
  else
    sock_fd6 = -1;
#endif

  if (sock_fd4 < 0
#ifdef HAVE_IPV6
      && sock_fd6 < 0
#endif
      ) {
    LOG_FATAL(LOGF_CmdMon, "Could not open any command socket");
  }

  access_auth_table = ADF_CreateTable();

}
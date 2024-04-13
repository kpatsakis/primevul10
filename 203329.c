process_cmd_clients(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  unsigned long next_index;
  int j;
  IPAddr ip;
  unsigned long client_hits;
  unsigned long peer_hits;
  unsigned long cmd_hits_auth;
  unsigned long cmd_hits_normal;
  unsigned long cmd_hits_bad;
  unsigned long last_ntp_hit_ago;
  unsigned long last_cmd_hit_ago;
  char hostname_buf[50];

  int n_replies;
  int n_indices_in_table;

  next_index = 0;

  printf("Hostname                   Client    Peer CmdAuth CmdNorm  CmdBad  LstN  LstC\n"
         "=========================  ======  ======  ======  ======  ======  ====  ====\n");

  do {

    request.command = htons(REQ_CLIENT_ACCESSES_BY_INDEX);
    request.data.client_accesses_by_index.first_index = htonl(next_index);
    request.data.client_accesses_by_index.n_indices = htonl(MAX_CLIENT_ACCESSES);

    if (request_reply(&request, &reply, RPY_CLIENT_ACCESSES_BY_INDEX, 0)) {
          n_replies = ntohl(reply.data.client_accesses_by_index.n_clients);
          n_indices_in_table = ntohl(reply.data.client_accesses_by_index.n_indices);
          if (n_replies == 0) {
            goto finished;
          }
          for (j=0; j<n_replies; j++) {
            UTI_IPNetworkToHost(&reply.data.client_accesses_by_index.clients[j].ip, &ip);
            if (ip.family != IPADDR_UNSPEC) {
              /* UNSPEC implies that the node could not be found in
                 the daemon's tables; we shouldn't ever generate this
                 case, but ignore it if we do.  (In future there might
                 be a protocol to reset the client logging; if another
                 administrator runs that while we're doing the clients
                 command, there will be a race condition that could
                 cause this). */
              
              client_hits = ntohl(reply.data.client_accesses_by_index.clients[j].client_hits);
              peer_hits = ntohl(reply.data.client_accesses_by_index.clients[j].peer_hits);
              cmd_hits_auth = ntohl(reply.data.client_accesses_by_index.clients[j].cmd_hits_auth);
              cmd_hits_normal = ntohl(reply.data.client_accesses_by_index.clients[j].cmd_hits_normal);
              cmd_hits_bad = ntohl(reply.data.client_accesses_by_index.clients[j].cmd_hits_bad);
              last_ntp_hit_ago = ntohl(reply.data.client_accesses_by_index.clients[j].last_ntp_hit_ago);
              last_cmd_hit_ago = ntohl(reply.data.client_accesses_by_index.clients[j].last_cmd_hit_ago);

              if (no_dns) {
                snprintf(hostname_buf, sizeof(hostname_buf),
                         "%s", UTI_IPToString(&ip));
              } else {
                DNS_IPAddress2Name(&ip, hostname_buf, sizeof(hostname_buf));
                hostname_buf[25] = 0;
              }
              printf("%-25s  %6ld  %6ld  %6ld  %6ld  %6ld  ",
                     hostname_buf,
                     client_hits, peer_hits,
                     cmd_hits_auth, cmd_hits_normal, cmd_hits_bad);
              print_seconds(last_ntp_hit_ago);
              printf("  ");
              print_seconds(last_cmd_hit_ago);
              printf("\n");
            }
          }              

          /* Set the next index to probe based on what the server tells us */
          next_index = ntohl(reply.data.client_accesses_by_index.next_index);
          if (next_index >= n_indices_in_table) {
            goto finished;
          }
    } else {
      return 0;
    }
  } while (1); /* keep going until all subnets have been expanded,
                  down to single nodes */

finished:
  return 1;
}
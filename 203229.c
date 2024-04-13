    if (request_reply(&request, &reply, RPY_CLIENT_ACCESSES, 0)) {
          n_replies = ntohl(reply.data.client_accesses.n_clients);
          for (j=0; j<n_replies; j++) {
            ip = ntohl(reply.data.client_accesses.clients[j].ip);
            if (ip != 0UL) {
              /* ip == 0 implies that the node could not be found in
                 the daemon's tables; we shouldn't ever generate this
                 case, but ignore it if we do.  (In future there might
                 be a protocol to reset the client logging; if another
                 administrator runs that while we're doing the clients
                 command, there will be a race condition that could
                 cause this). */
              
              client_hits = ntohl(reply.data.client_accesses.clients[j].client_hits);
              peer_hits = ntohl(reply.data.client_accesses.clients[j].peer_hits);
              cmd_hits_auth = ntohl(reply.data.client_accesses.clients[j].cmd_hits_auth);
              cmd_hits_normal = ntohl(reply.data.client_accesses.clients[j].cmd_hits_normal);
              cmd_hits_bad = ntohl(reply.data.client_accesses.clients[j].cmd_hits_bad);
              last_ntp_hit_ago = ntohl(reply.data.client_accesses.clients[j].last_ntp_hit_ago);
              last_cmd_hit_ago = ntohl(reply.data.client_accesses.clients[j].last_cmd_hit_ago);

              if (no_dns) {
                snprintf(hostname_buf, sizeof(hostname_buf), 
                         "%s", UTI_IPToDottedQuad(ip));
              } else {
                DNS_IPAddress2Name(ip, hostname_buf, sizeof(hostname_buf));
                hostname_buf[25] = 0;
              }
              printf("%-25s  %6d  %6d  %6d  %6d  %6d  ",
                     hostname_buf,
                     client_hits, peer_hits,
                     cmd_hits_auth, cmd_hits_normal, cmd_hits_bad);
              print_seconds(last_ntp_hit_ago);
              printf("  ");
              print_seconds(last_cmd_hit_ago);
              printf("\n");
            }
          }              

          /* Skip the todo pointer forwards by the number of nets looked
             up.  Can't do this earlier, because we might have to point
             at the next layer of subnets that have only just been
             concatenated to the linked list. */
          for (i=0; i<clients_looked_up; i++) {
            todo = todo->next;
          }

    }
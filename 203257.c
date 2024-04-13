process_cmd_clients(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  SubnetToDo *head, *todo, *tail, *p, *next_node, *new_node;
  int i, j, nets_looked_up, clients_looked_up;
  int word;
  unsigned long mask;
  unsigned long ip, bits;
  unsigned long client_hits;
  unsigned long peer_hits;
  unsigned long cmd_hits_auth;
  unsigned long cmd_hits_normal;
  unsigned long cmd_hits_bad;
  unsigned long last_ntp_hit_ago;
  unsigned long last_cmd_hit_ago;
  char hostname_buf[50];

  int n_replies;

  head = todo = MallocNew(SubnetToDo);
  todo->next = NULL;
  /* Set up initial query = root subnet */
  todo->ip = 0;
  todo->bits = 0;
  tail = todo;

  do {

    request.command = htons(REQ_SUBNETS_ACCESSED);
    /* Build list of subnets to examine */
    i=0;
    p=todo;
    while((i < MAX_SUBNETS_ACCESSED) &&
          p &&
          (p->bits < 32)) {
      
      request.data.subnets_accessed.subnets[i].ip = htonl(p->ip);
      request.data.subnets_accessed.subnets[i].bits_specd = htonl(p->bits);
      p = p->next;
      i++;
    }         

    nets_looked_up = i;

    if (nets_looked_up == 0) {
      /* No subnets need examining */
      break;
    }

    request.data.subnets_accessed.n_subnets = htonl(nets_looked_up);

    if (request_reply(&request, &reply, RPY_SUBNETS_ACCESSED, 0)) {
          n_replies = ntohl(reply.data.subnets_accessed.n_subnets);
          for (j=0; j<n_replies; j++) {
            ip = ntohl(reply.data.subnets_accessed.subnets[j].ip);
            bits = ntohl(reply.data.subnets_accessed.subnets[j].bits_specd);
            for (i=0; i<256; i++) {
              word = i/32;
              mask = 1UL << (i%32);
              if (ntohl(reply.data.subnets_accessed.subnets[j].bitmap[word]) & mask) {
                /* Add this subnet to the todo list */
                new_node = MallocNew(SubnetToDo);
                new_node->next = NULL;
                new_node->bits = bits + 8;
                new_node->ip = ip | (i << (24 - bits));
                tail->next = new_node;
                tail = new_node;
#if 0
                printf("%08lx %2d %3d %08lx\n", ip, bits, i, new_node->ip);
#endif
              }
            }
          }

          /* Skip the todo pointer forwards by the number of nets looked
             up.  Can't do this earlier, because we might have to point
             at the next layer of subnets that have only just been
             concatenated to the linked list. */
          for (i=0; i<nets_looked_up; i++) {
            todo = todo->next;
          }
          
      }
      
    } else {
      return;
    }
  } while (1); /* keep going until all subnets have been expanded,
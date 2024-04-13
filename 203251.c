    while ((i < MAX_CLIENT_ACCESSES) &&
           p) {

      request.data.client_accesses.client_ips[i] = htonl(p->ip);
      p = p->next;
      i++;
    }
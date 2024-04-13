handle_client_accesses_by_index(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  CLG_Status result;
  RPT_ClientAccessByIndex_Report report;
  unsigned long first_index, n_indices, last_index, n_indices_in_table;
  int i, j;
  struct timeval now;

  LCL_ReadCookedTime(&now, NULL);

  first_index = ntohl(rx_message->data.client_accesses_by_index.first_index);
  n_indices = ntohl(rx_message->data.client_accesses_by_index.n_indices);
  last_index = first_index + n_indices - 1;

  tx_message->status = htons(STT_SUCCESS);
  tx_message->reply = htons(RPY_CLIENT_ACCESSES_BY_INDEX);

  for (i = first_index, j = 0;
       (i <= last_index) && (j < MAX_CLIENT_ACCESSES);
       i++) {

    result = CLG_GetClientAccessReportByIndex(i, &report, now.tv_sec, &n_indices_in_table);
    tx_message->data.client_accesses_by_index.n_indices = htonl(n_indices_in_table);

    switch (result) {
      case CLG_SUCCESS:
        UTI_IPHostToNetwork(&report.ip_addr, &tx_message->data.client_accesses_by_index.clients[j].ip);
        tx_message->data.client_accesses_by_index.clients[j].client_hits = htonl(report.client_hits);
        tx_message->data.client_accesses_by_index.clients[j].peer_hits = htonl(report.peer_hits);
        tx_message->data.client_accesses_by_index.clients[j].cmd_hits_auth = htonl(report.cmd_hits_auth);
        tx_message->data.client_accesses_by_index.clients[j].cmd_hits_normal = htonl(report.cmd_hits_normal);
        tx_message->data.client_accesses_by_index.clients[j].cmd_hits_bad = htonl(report.cmd_hits_bad);
        tx_message->data.client_accesses_by_index.clients[j].last_ntp_hit_ago = htonl(report.last_ntp_hit_ago);
        tx_message->data.client_accesses_by_index.clients[j].last_cmd_hit_ago = htonl(report.last_cmd_hit_ago);
        j++;
        break;
      case CLG_INDEXTOOLARGE:
        break; /* ignore this index */
      case CLG_INACTIVE:
        tx_message->status = htons(STT_INACTIVE);
        return;
      default:
        assert(0);
        break;
    }
  }

  tx_message->data.client_accesses_by_index.next_index = htonl(i);
  tx_message->data.client_accesses_by_index.n_clients = htonl(j);
}
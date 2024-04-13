NCR_ProcessUnknown
(NTP_Packet *message,           /* the received message */
 struct timeval *now,           /* timestamp at time of receipt */
 double now_err,                /* assumed error in the timestamp */
 NTP_Remote_Address *remote_addr,
 NTP_Local_Address *local_addr,
 int length                     /* the length of the received packet */
 )
{
  NTP_Mode pkt_mode, my_mode;
  int has_auth, valid_auth;
  uint32_t key_id;

  /* Ignore the packet if it wasn't received by server socket */
  if (!NIO_IsServerSocket(local_addr->sock_fd)) {
    DEBUG_LOG(LOGF_NtpCore, "NTP request packet received by client socket %d",
              local_addr->sock_fd);
    return;
  }

  if (!check_packet_format(message, length))
    return;

  if (!ADF_IsAllowed(access_auth_table, &remote_addr->ip_addr)) {
    DEBUG_LOG(LOGF_NtpCore, "NTP packet received from unauthorised host %s port %d",
              UTI_IPToString(&remote_addr->ip_addr),
              remote_addr->port);
    return;
  }

  pkt_mode = NTP_LVM_TO_MODE(message->lvm);

  switch (pkt_mode) {
    case MODE_ACTIVE:
      /* We are symmetric passive, even though we don't ever lock to him */
      my_mode = MODE_PASSIVE;
      CLG_LogNTPPeerAccess(&remote_addr->ip_addr, now->tv_sec);
      break;
    case MODE_CLIENT:
      /* Reply with server packet */
      my_mode = MODE_SERVER;
      CLG_LogNTPClientAccess(&remote_addr->ip_addr, now->tv_sec);
      break;
    default:
      /* Discard */
      DEBUG_LOG(LOGF_NtpCore, "NTP packet discarded pkt_mode=%d", pkt_mode);
      return;
  }

  /* Check if the packet includes MAC that authenticates properly */
  valid_auth = check_packet_auth(message, length, &has_auth, &key_id);

  /* If authentication failed, reply with crypto-NAK */
  if (!valid_auth)
    key_id = 0;

  /* Send a reply.
     - copy the poll value as the client may use it to control its polling
       interval
     - authenticate the packet if the request was authenticated
     - originate timestamp is the client's transmit time
     - don't save our transmit timestamp as we aren't maintaining state about
       this client */
  transmit_packet(my_mode, message->poll, NTP_LVM_TO_VERSION(message->lvm),
                  has_auth, key_id, &message->transmit_ts, now, NULL, NULL,
                  remote_addr, local_addr);
}
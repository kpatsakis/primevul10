NCR_ProcessKnown
(NTP_Packet *message,           /* the received message */
 struct timeval *now,           /* timestamp at time of receipt */
 double now_err,
 NCR_Instance inst,             /* the instance record for this peer/server */
 NTP_Local_Address *local_addr, /* the receiving address */
 int length                     /* the length of the received packet */
 )
{
  int pkt_mode, proc_packet, proc_as_unknown, log_peer_access;

  if (!check_packet_format(message, length))
    return 0;

  pkt_mode = NTP_LVM_TO_MODE(message->lvm);
  proc_packet = 0;
  proc_as_unknown = 0;
  log_peer_access = 0;

  /* Now, depending on the mode we decide what to do */
  switch (pkt_mode) {
    case MODE_ACTIVE:
      switch (inst->mode) {
        case MODE_ACTIVE:
          /* Ordinary symmetric peering */
          log_peer_access = 1;
          proc_packet = 1;
          break;
        case MODE_PASSIVE:
          /* In this software this case should not arise, we don't
             support unconfigured peers */
          break;
        case MODE_CLIENT:
          /* This is where we have the remote configured as a server and he has
             us configured as a peer, process as from an unknown source */
          proc_as_unknown = 1;
          break;
        default:
          /* Discard */
          break;
      }
      break;

    case MODE_PASSIVE:
      switch (inst->mode) {
        case MODE_ACTIVE:
          /* This would arise if we have the remote configured as a peer and
             he does not have us configured */
          log_peer_access = 1;
          proc_packet = 1;
          break;
        case MODE_PASSIVE:
          /* Error condition in RFC 5905 */
          break;
        default:
          /* Discard */
          break;
      }
      break;

    case MODE_CLIENT:
      /* If message is client mode, we just respond with a server mode
         packet, regardless of what we think the remote machine is
         supposed to be.  However, even though this is a configured
         peer or server, we still implement access restrictions on
         client mode operation.

         This copes with the case for an isolated network where one
         machine is set by eye and is used as the master, with the
         other machines pointed at it.  If the master goes down, we
         want to be able to reset its time at startup by relying on
         one of the secondaries to flywheel it. The behaviour coded here
         is required in the secondaries to make this possible. */

      proc_as_unknown = 1;
      break;

    case MODE_SERVER:
      /* Ignore presend reply */
      if (inst->presend_done)
        break;

      switch (inst->mode) {
        case MODE_CLIENT:
          /* Standard case where he's a server and we're the client */
          proc_packet = 1;
          break;
        default:
          /* Discard */
          break;
      }
      break;

    case MODE_BROADCAST:
      /* Just ignore these */
      break;

    default:
      /* Obviously ignore */
      break;
  }

  if (log_peer_access)
    CLG_LogNTPPeerAccess(&inst->remote_addr.ip_addr, now->tv_sec);

  if (proc_packet) {
    /* Check if the reply was received by the socket that sent the request */
    if (local_addr->sock_fd != inst->local_addr.sock_fd) {
      DEBUG_LOG(LOGF_NtpCore,
                "Packet received by wrong socket %d (expected %d)",
                local_addr->sock_fd, inst->local_addr.sock_fd);
      return 0;
    }

    /* Ignore packets from offline sources */
    if (inst->opmode == MD_OFFLINE || inst->tx_suspended) {
      DEBUG_LOG(LOGF_NtpCore, "Packet from offline source");
      return 0;
    }

    return receive_packet(message, now, now_err, inst, local_addr, length);
  } else if (proc_as_unknown) {
    NCR_ProcessUnknown(message, now, now_err, &inst->remote_addr,
                       local_addr, length);
    /* It's not a reply to our request, don't return success */
    return 0;
  } else {
    DEBUG_LOG(LOGF_NtpCore, "NTP packet discarded pkt_mode=%d our_mode=%d",
              pkt_mode, inst->mode);
    return 0;
  }
}
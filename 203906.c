transmit_timeout(void *arg)
{
  NCR_Instance inst = (NCR_Instance) arg;
  int sent;

  inst->timer_running = 0;

  switch (inst->opmode) {
    case MD_BURST_WAS_ONLINE:
      /* With online burst switch to online before last packet */
      if (inst->burst_total_samples_to_go <= 1)
        inst->opmode = MD_ONLINE;
    case MD_BURST_WAS_OFFLINE:
      if (inst->burst_total_samples_to_go <= 0)
        take_offline(inst);
      break;
    default:
      break;
  }

  /* With auto_offline take the source offline on 2nd missed reply */
  if (inst->auto_offline && inst->tx_count >= 2)
    NCR_TakeSourceOffline(inst);

  if (inst->opmode == MD_OFFLINE) {
    return;
  }

  DEBUG_LOG(LOGF_NtpCore, "Transmit timeout for [%s:%d]",
      UTI_IPToString(&inst->remote_addr.ip_addr), inst->remote_addr.port);

  /* Open new client socket */
  if (inst->mode == MODE_CLIENT) {
    close_client_socket(inst);
    assert(inst->local_addr.sock_fd == INVALID_SOCK_FD);
    inst->local_addr.sock_fd = NIO_OpenClientSocket(&inst->remote_addr);
  }

  /* Check whether we need to 'warm up' the link to the other end by
     sending an NTP exchange to ensure both ends' ARP caches are
     primed.  On loaded systems this might also help ensure that bits
     of the program are paged in properly before we start. */

  if ((inst->presend_minpoll > 0) &&
      (inst->presend_minpoll <= inst->local_poll) &&
      !inst->presend_done) {
    
    /* Send a client packet, don't store the local tx values
       as the reply will be ignored */
    transmit_packet(MODE_CLIENT, inst->local_poll, inst->version, 0, 0,
                    &inst->remote_orig, &inst->local_rx, NULL, NULL,
                    &inst->remote_addr, &inst->local_addr);

    inst->presend_done = 1;

    /* Requeue timeout */
    restart_timeout(inst, WARM_UP_DELAY);

    return;
  }

  inst->presend_done = 0; /* Reset for next time */

  sent = transmit_packet(inst->mode, inst->local_poll,
                         inst->version,
                         inst->do_auth, inst->auth_key_id,
                         &inst->remote_orig,
                         &inst->local_rx, &inst->local_tx, &inst->local_ntp_tx,
                         &inst->remote_addr,
                         &inst->local_addr);

  ++inst->tx_count;

  /* If the source loses connectivity and our packets are still being sent,
     back off the sampling rate to reduce the network traffic.  If it's the
     source to which we are currently locked, back off slowly. */

  if (inst->tx_count >= 2) {
    /* Implies we have missed at least one transmission */

    if (sent) {
      adjust_poll(inst, SRC_IsSyncPeer(inst->source) ? 0.1 : 0.25);
    }

    SRC_UpdateReachability(inst->source, 0);
  }

  switch (inst->opmode) {
    case MD_BURST_WAS_ONLINE:
      /* When not reachable, don't stop online burst until sending succeeds */
      if (!sent && !SRC_IsReachable(inst->source))
        break;
      /* Fall through */
    case MD_BURST_WAS_OFFLINE:
      --inst->burst_total_samples_to_go;
      break;
    default:
      break;
  }

  /* Restart timer for this message */
  restart_timeout(inst, get_transmit_delay(inst, 1, 0.0));
}
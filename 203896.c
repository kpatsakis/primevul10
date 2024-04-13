get_transmit_delay(NCR_Instance inst, int on_tx, double last_tx)
{
  int poll_to_use, stratum_diff;
  double delay_time;

  /* If we're in burst mode, queue for immediate dispatch.

     If we're operating in client/server mode, queue the timeout for
     the poll interval hence.  The fact that a timeout has been queued
     in the transmit handler is immaterial - that is only done so that
     we at least send something, if no reply is heard.

     If we're in symmetric mode, we have to take account of the peer's
     wishes, otherwise his sampling regime will fall to pieces.  If
     we're in client/server mode, we don't care what poll interval the
     server responded with last time. */

  switch (inst->opmode) {
    case MD_OFFLINE:
      assert(0);
      break;
    case MD_ONLINE:
      /* Normal processing, depending on whether we're in
         client/server or symmetric mode */

      switch(inst->mode) {
        case MODE_CLIENT:
          /* Client/server association - aim at some randomised time
             approx the poll interval away */
          poll_to_use = inst->local_poll;

          delay_time = (double) (1UL<<poll_to_use);

          break;

        case MODE_ACTIVE:
          /* Symmetric active association - aim at some randomised time approx
             the poll interval away since the last transmit */

          /* Use shorter of the local and remote poll interval, but not shorter
             than the allowed minimum */
          poll_to_use = inst->local_poll;
          if (poll_to_use > inst->remote_poll)
            poll_to_use = inst->remote_poll;
          if (poll_to_use < inst->minpoll)
            poll_to_use = inst->minpoll;

          delay_time = (double) (1UL<<poll_to_use);

          /* If the remote stratum is higher than ours, try to lock on the
             peer's polling to minimize our response time by slightly extending
             our delay or waiting for the peer to catch up with us as the
             random part in the actual interval is reduced. If the remote
             stratum is equal to ours, try to interleave evenly with the peer. */
          stratum_diff = inst->remote_stratum - REF_GetOurStratum();
          if ((stratum_diff > 0 && last_tx * PEER_SAMPLING_ADJ < delay_time) ||
              (!on_tx && !stratum_diff &&
               last_tx / delay_time > PEER_SAMPLING_ADJ - 0.5))
            delay_time *= PEER_SAMPLING_ADJ;

          /* Substract the already spend time */
          if (last_tx > 0.0)
            delay_time -= last_tx;
          if (delay_time < 0.0)
            delay_time = 0.0;

          break;
        default:
          assert(0);
          break;
      }
      break;

    case MD_BURST_WAS_ONLINE:
    case MD_BURST_WAS_OFFLINE:
      /* Burst modes */
      delay_time = on_tx ? BURST_TIMEOUT : BURST_INTERVAL;
      break;
    default:
      assert(0);
      break;
  }

  return delay_time;
}
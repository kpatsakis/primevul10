NCR_ResetInstance(NCR_Instance instance)
{
  instance->tx_count = 0;
  instance->presend_done = 0;

  instance->poll_score = 0.0;
  instance->remote_poll = 0;
  instance->remote_stratum = 0;

  instance->remote_orig.hi = 0;
  instance->remote_orig.lo = 0;
  instance->local_rx.tv_sec = 0;
  instance->local_rx.tv_usec = 0;
  instance->local_tx.tv_sec = 0;
  instance->local_tx.tv_usec = 0;
  instance->local_ntp_tx.hi = 0;
  instance->local_ntp_tx.lo = 0;

  if (instance->local_poll != instance->minpoll) {
    instance->local_poll = instance->minpoll;

    /* The timer was set with a longer poll interval, restart it */
    if (instance->timer_running)
      restart_timeout(instance, get_transmit_delay(instance, 0, 0.0));
  }
}
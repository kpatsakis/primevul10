restart_timeout(NCR_Instance inst, double delay)
{
  /* Check if we can transmit */
  if (inst->tx_suspended) {
    assert(!inst->timer_running);
    return;
  }

  /* Stop old timer if running */
  if (inst->timer_running)
    SCH_RemoveTimeout(inst->timeout_id);

  /* Start new timer for transmission */
  inst->timeout_id = SCH_AddTimeoutInClass(delay, SAMPLING_SEPARATION,
                                           SAMPLING_RANDOMNESS,
                                           SCH_NtpSamplingClass,
                                           transmit_timeout, (void *)inst);
  inst->timer_running = 1;
}
NCR_SlewTimes(NCR_Instance inst, struct timeval *when, double dfreq, double doffset)
{
  double delta;

  if (inst->local_rx.tv_sec || inst->local_rx.tv_usec)
    UTI_AdjustTimeval(&inst->local_rx, when, &inst->local_rx, &delta, dfreq, doffset);
  if (inst->local_tx.tv_sec || inst->local_tx.tv_usec)
    UTI_AdjustTimeval(&inst->local_tx, when, &inst->local_tx, &delta, dfreq, doffset);
}
NCR_StartInstance(NCR_Instance instance)
{
  instance->tx_suspended = 0;
  if (instance->opmode != MD_OFFLINE)
    start_initial_timeout(instance);
}
NCR_ModifyMaxdelay(NCR_Instance inst, double new_max_delay)
{
  inst->max_delay = new_max_delay;
  LOG(LOGS_INFO, LOGF_NtpCore, "Source %s new max delay %f",
      UTI_IPToString(&inst->remote_addr.ip_addr), new_max_delay);
}
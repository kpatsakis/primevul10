NCR_ModifyMaxdelayratio(NCR_Instance inst, double new_max_delay_ratio)
{
  inst->max_delay_ratio = new_max_delay_ratio;
  LOG(LOGS_INFO, LOGF_NtpCore, "Source %s new max delay ratio %f",
      UTI_IPToString(&inst->remote_addr.ip_addr), new_max_delay_ratio);
}
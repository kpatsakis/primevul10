NCR_ModifyMinstratum(NCR_Instance inst, int new_min_stratum)
{
  inst->min_stratum = new_min_stratum;
  LOG(LOGS_INFO, LOGF_NtpCore, "Source %s new minstratum %d",
      UTI_IPToString(&inst->remote_addr.ip_addr), new_min_stratum);
}
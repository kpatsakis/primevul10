NCR_TakeSourceOnline(NCR_Instance inst)
{
  switch (inst->opmode) {
    case MD_ONLINE:
      /* Nothing to do */
      break;
    case MD_OFFLINE:
      LOG(LOGS_INFO, LOGF_NtpCore, "Source %s online", UTI_IPToString(&inst->remote_addr.ip_addr));
      inst->opmode = MD_ONLINE;
      NCR_ResetInstance(inst);
      start_initial_timeout(inst);
      break;
    case MD_BURST_WAS_ONLINE:
      /* Will revert */
      break;
    case MD_BURST_WAS_OFFLINE:
      inst->opmode = MD_BURST_WAS_ONLINE;
      LOG(LOGS_INFO, LOGF_NtpCore, "Source %s online", UTI_IPToString(&inst->remote_addr.ip_addr));
      break;
  }
}
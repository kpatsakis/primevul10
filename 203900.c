NCR_TakeSourceOffline(NCR_Instance inst)
{
  switch (inst->opmode) {
    case MD_ONLINE:
      LOG(LOGS_INFO, LOGF_NtpCore, "Source %s offline", UTI_IPToString(&inst->remote_addr.ip_addr));
      take_offline(inst);
      break;
    case MD_OFFLINE:
      break;
    case MD_BURST_WAS_ONLINE:
      inst->opmode = MD_BURST_WAS_OFFLINE;
      LOG(LOGS_INFO, LOGF_NtpCore, "Source %s offline", UTI_IPToString(&inst->remote_addr.ip_addr));
      break;
    case MD_BURST_WAS_OFFLINE:
      break;
  }

}
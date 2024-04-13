NCR_ModifyPolltarget(NCR_Instance inst, int new_poll_target)
{
  inst->poll_target = new_poll_target;
  LOG(LOGS_INFO, LOGF_NtpCore, "Source %s new polltarget %d",
      UTI_IPToString(&inst->remote_addr.ip_addr), new_poll_target);
}
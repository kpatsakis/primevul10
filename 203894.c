NCR_ModifyMinpoll(NCR_Instance inst, int new_minpoll)
{
  if (new_minpoll < MIN_POLL || new_minpoll > MAX_POLL)
    return;
  inst->minpoll = new_minpoll;
  LOG(LOGS_INFO, LOGF_NtpCore, "Source %s new minpoll %d", UTI_IPToString(&inst->remote_addr.ip_addr), new_minpoll);
  if (inst->maxpoll < inst->minpoll)
    NCR_ModifyMaxpoll(inst, inst->minpoll);
}
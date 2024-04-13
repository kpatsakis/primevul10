NCR_ModifyMaxpoll(NCR_Instance inst, int new_maxpoll)
{
  if (new_maxpoll < MIN_POLL || new_maxpoll > MAX_POLL)
    return;
  inst->maxpoll = new_maxpoll;
  LOG(LOGS_INFO, LOGF_NtpCore, "Source %s new maxpoll %d", UTI_IPToString(&inst->remote_addr.ip_addr), new_maxpoll);
  if (inst->minpoll > inst->maxpoll)
    NCR_ModifyMinpoll(inst, inst->maxpoll);
}
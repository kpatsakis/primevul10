NCR_GetInstance(NTP_Remote_Address *remote_addr, NTP_Source_Type type, SourceParameters *params)
{
  NCR_Instance result;

  result = MallocNew(struct NCR_Instance_Record);

  result->remote_addr = *remote_addr;
  result->local_addr.ip_addr.family = IPADDR_UNSPEC;

  switch (type) {
    case NTP_SERVER:
      /* Client socket will be obtained when sending request */
      result->local_addr.sock_fd = INVALID_SOCK_FD;
      result->mode = MODE_CLIENT;
      break;
    case NTP_PEER:
      result->local_addr.sock_fd = NIO_OpenServerSocket(remote_addr);
      result->mode = MODE_ACTIVE;
      break;
    default:
      assert(0);
  }

  result->minpoll = params->minpoll;
  if (result->minpoll < MIN_POLL)
    result->minpoll = SRC_DEFAULT_MINPOLL;
  else if (result->minpoll > MAX_POLL)
    result->minpoll = MAX_POLL;
  result->maxpoll = params->maxpoll;
  if (result->maxpoll < MIN_POLL)
    result->maxpoll = SRC_DEFAULT_MAXPOLL;
  else if (result->maxpoll > MAX_POLL)
    result->maxpoll = MAX_POLL;
  if (result->maxpoll < result->minpoll)
    result->maxpoll = result->minpoll;

  result->min_stratum = params->min_stratum;
  if (result->min_stratum >= NTP_MAX_STRATUM)
    result->min_stratum = NTP_MAX_STRATUM - 1;
  result->presend_minpoll = params->presend_minpoll;

  result->max_delay = params->max_delay;
  result->max_delay_ratio = params->max_delay_ratio;
  result->max_delay_dev_ratio = params->max_delay_dev_ratio;
  result->auto_offline = params->auto_offline;
  result->poll_target = params->poll_target;

  result->version = params->version;
  if (result->version < NTP_MIN_COMPAT_VERSION)
    result->version = NTP_MIN_COMPAT_VERSION;
  else if (result->version > NTP_VERSION)
    result->version = NTP_VERSION;

  if (params->authkey == INACTIVE_AUTHKEY) {
    result->do_auth = 0;
    result->auth_key_id = 0;
  } else {
    result->do_auth = 1;
    result->auth_key_id = params->authkey;
    if (!KEY_KeyKnown(result->auth_key_id)) {
      LOG(LOGS_WARN, LOGF_NtpCore, "Source %s added with unknown key %"PRIu32,
          UTI_IPToString(&result->remote_addr.ip_addr), result->auth_key_id);
    }
  }

  /* Create a source instance for this NTP source */
  result->source = SRC_CreateNewInstance(UTI_IPToRefid(&remote_addr->ip_addr),
                                         SRC_NTP, params->sel_option,
                                         &result->remote_addr.ip_addr,
                                         params->min_samples, params->max_samples);

  result->timer_running = 0;
  result->timeout_id = 0;
  result->tx_suspended = 1;
  result->opmode = params->online ? MD_ONLINE : MD_OFFLINE;
  result->local_poll = result->minpoll;
  
  NCR_ResetInstance(result);

  if (params->iburst) {
    NCR_InitiateSampleBurst(result, IBURST_GOOD_SAMPLES, IBURST_TOTAL_SAMPLES);
  }

  return result;
}
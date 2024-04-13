NCR_ReportSource(NCR_Instance inst, RPT_SourceReport *report, struct timeval *now)
{
  report->poll = inst->local_poll;

  switch (inst->mode) {
    case MODE_CLIENT:
      report->mode = RPT_NTP_CLIENT;
      break;
    case MODE_ACTIVE:
      report->mode = RPT_NTP_PEER;
      break;
    default:
      assert(0);
  }
}
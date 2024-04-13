handle_rtcreport(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  RPT_RTC_Report report;
  status = RTC_GetReport(&report);
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
    tx_message->reply  = htons(RPY_RTC);
    UTI_TimevalHostToNetwork(&report.ref_time, &tx_message->data.rtc.ref_time);
    tx_message->data.rtc.n_samples = htons(report.n_samples);
    tx_message->data.rtc.n_runs = htons(report.n_runs);
    tx_message->data.rtc.span_seconds = htonl(report.span_seconds);
    tx_message->data.rtc.rtc_seconds_fast = UTI_FloatHostToNetwork(report.rtc_seconds_fast);
    tx_message->data.rtc.rtc_gain_rate_ppm = UTI_FloatHostToNetwork(report.rtc_gain_rate_ppm);
  } else {
    tx_message->status = htons(STT_NORTC);
  }
}
handle_sourcestats(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  RPT_SourcestatsReport report;
  struct timeval now_corr;

  LCL_ReadCookedTime(&now_corr, NULL);
  status = SRC_ReportSourcestats(ntohl(rx_message->data.sourcestats.index),
                                 &report, &now_corr);

  if (status) {
    tx_message->status = htons(STT_SUCCESS);
    tx_message->reply = htons(RPY_SOURCESTATS);
    tx_message->data.sourcestats.ref_id = htonl(report.ref_id);
    UTI_IPHostToNetwork(&report.ip_addr, &tx_message->data.sourcestats.ip_addr);
    tx_message->data.sourcestats.n_samples = htonl(report.n_samples);
    tx_message->data.sourcestats.n_runs = htonl(report.n_runs);
    tx_message->data.sourcestats.span_seconds = htonl(report.span_seconds);
    tx_message->data.sourcestats.resid_freq_ppm = UTI_FloatHostToNetwork(report.resid_freq_ppm);
    tx_message->data.sourcestats.skew_ppm = UTI_FloatHostToNetwork(report.skew_ppm);
    tx_message->data.sourcestats.sd = UTI_FloatHostToNetwork(report.sd);
    tx_message->data.sourcestats.est_offset = UTI_FloatHostToNetwork(report.est_offset);
    tx_message->data.sourcestats.est_offset_err = UTI_FloatHostToNetwork(report.est_offset_err);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
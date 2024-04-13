handle_source_data(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  RPT_SourceReport report;
  struct timeval now_corr;

  /* Get data */
  LCL_ReadCookedTime(&now_corr, NULL);
  if (SRC_ReportSource(ntohl(rx_message->data.source_data.index), &report, &now_corr)) {
    switch (SRC_GetType(ntohl(rx_message->data.source_data.index))) {
      case SRC_NTP:
        NSR_ReportSource(&report, &now_corr);
        break;
      case SRC_REFCLOCK:
        RCL_ReportSource(&report, &now_corr);
        break;
    }
    
    tx_message->status = htons(STT_SUCCESS);
    tx_message->reply  = htons(RPY_SOURCE_DATA);
    
    UTI_IPHostToNetwork(&report.ip_addr, &tx_message->data.source_data.ip_addr);
    tx_message->data.source_data.stratum = htons(report.stratum);
    tx_message->data.source_data.poll    = htons(report.poll);
    switch (report.state) {
      case RPT_SYNC:
        tx_message->data.source_data.state   = htons(RPY_SD_ST_SYNC);
        break;
      case RPT_UNREACH:
        tx_message->data.source_data.state   = htons(RPY_SD_ST_UNREACH);
        break;
      case RPT_FALSETICKER:
        tx_message->data.source_data.state   = htons(RPY_SD_ST_FALSETICKER);
        break;
      case RPT_JITTERY:
        tx_message->data.source_data.state   = htons(RPY_SD_ST_JITTERY);
        break;
      case RPT_CANDIDATE:
        tx_message->data.source_data.state   = htons(RPY_SD_ST_CANDIDATE);
        break;
      case RPT_OUTLIER:
        tx_message->data.source_data.state   = htons(RPY_SD_ST_OUTLIER);
        break;
    }
    switch (report.mode) {
      case RPT_NTP_CLIENT:
        tx_message->data.source_data.mode    = htons(RPY_SD_MD_CLIENT);
        break;
      case RPT_NTP_PEER:
        tx_message->data.source_data.mode    = htons(RPY_SD_MD_PEER);
        break;
      case RPT_LOCAL_REFERENCE:
        tx_message->data.source_data.mode    = htons(RPY_SD_MD_REF);
        break;
    }
    switch (report.sel_option) {
      case RPT_NORMAL:
        tx_message->data.source_data.flags = htons(0);
        break;
      case RPT_PREFER:
        tx_message->data.source_data.flags = htons(RPY_SD_FLAG_PREFER);
        break;
      case RPT_NOSELECT:
        tx_message->data.source_data.flags = htons(RPY_SD_FLAG_PREFER);
        break;
    }
    tx_message->data.source_data.reachability = htons(report.reachability);
    tx_message->data.source_data.since_sample = htonl(report.latest_meas_ago);
    tx_message->data.source_data.orig_latest_meas = UTI_FloatHostToNetwork(report.orig_latest_meas);
    tx_message->data.source_data.latest_meas = UTI_FloatHostToNetwork(report.latest_meas);
    tx_message->data.source_data.latest_meas_err = UTI_FloatHostToNetwork(report.latest_meas_err);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
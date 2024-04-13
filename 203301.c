handle_tracking(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  RPT_TrackingReport rpt;

  REF_GetTrackingReport(&rpt);
  tx_message->status = htons(STT_SUCCESS);
  tx_message->reply  = htons(RPY_TRACKING);
  tx_message->data.tracking.ref_id = htonl(rpt.ref_id);
  UTI_IPHostToNetwork(&rpt.ip_addr, &tx_message->data.tracking.ip_addr);
  tx_message->data.tracking.stratum = htons(rpt.stratum);
  tx_message->data.tracking.leap_status = htons(rpt.leap_status);
  UTI_TimevalHostToNetwork(&rpt.ref_time, &tx_message->data.tracking.ref_time);
  tx_message->data.tracking.current_correction = UTI_FloatHostToNetwork(rpt.current_correction);
  tx_message->data.tracking.last_offset = UTI_FloatHostToNetwork(rpt.last_offset);
  tx_message->data.tracking.rms_offset = UTI_FloatHostToNetwork(rpt.rms_offset);
  tx_message->data.tracking.freq_ppm = UTI_FloatHostToNetwork(rpt.freq_ppm);
  tx_message->data.tracking.resid_freq_ppm = UTI_FloatHostToNetwork(rpt.resid_freq_ppm);
  tx_message->data.tracking.skew_ppm = UTI_FloatHostToNetwork(rpt.skew_ppm);
  tx_message->data.tracking.root_delay = UTI_FloatHostToNetwork(rpt.root_delay);
  tx_message->data.tracking.root_dispersion = UTI_FloatHostToNetwork(rpt.root_dispersion);
  tx_message->data.tracking.last_update_interval = UTI_FloatHostToNetwork(rpt.last_update_interval);
}
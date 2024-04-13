handle_settime(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  struct timeval ts;
  long offset_cs;
  double dfreq_ppm, new_afreq_ppm;
  UTI_TimevalNetworkToHost(&rx_message->data.settime.ts, &ts);
  if (MNL_AcceptTimestamp(&ts, &offset_cs, &dfreq_ppm, &new_afreq_ppm)) {
    tx_message->status = htons(STT_SUCCESS);
    tx_message->reply = htons(RPY_MANUAL_TIMESTAMP);
    tx_message->data.manual_timestamp.centiseconds = htonl((int32_t)offset_cs);
    tx_message->data.manual_timestamp.dfreq_ppm = UTI_FloatHostToNetwork(dfreq_ppm);
    tx_message->data.manual_timestamp.new_afreq_ppm = UTI_FloatHostToNetwork(new_afreq_ppm);
  } else {
    tx_message->status = htons(STT_NOTENABLED);
  }
}
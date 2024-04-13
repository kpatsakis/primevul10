handle_dfreq(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  double dfreq;
  dfreq = UTI_FloatNetworkToHost(rx_message->data.dfreq.dfreq);
  LCL_AccumulateDeltaFrequency(dfreq * 1.0e-6);
  LOG(LOGS_INFO, LOGF_CmdMon, "Accumulated delta freq of %.3fppm", dfreq);
  tx_message->status = htons(STT_SUCCESS);
}
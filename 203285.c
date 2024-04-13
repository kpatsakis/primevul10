handle_doffset(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  long sec, usec;
  double doffset;
  sec = (long)(ntohl(rx_message->data.doffset.sec));
  usec = (long)(ntohl(rx_message->data.doffset.usec));
  doffset = (double) sec + 1.0e-6 * (double) usec;
  LOG(LOGS_INFO, LOGF_CmdMon, "Accumulated delta offset of %.6f seconds", doffset);
  LCL_AccumulateOffset(doffset, 0.0);
  tx_message->status = htons(STT_SUCCESS);
}
handle_trimrtc(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  status = RTC_Trim();
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NORTC);
  }
}
handle_reselect(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  SRC_ReselectSource();
  tx_message->status = htons(STT_SUCCESS);
}
handle_cyclelogs(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  LOG_CycleLogFiles();
  
  tx_message->status = htons(STT_SUCCESS);
}
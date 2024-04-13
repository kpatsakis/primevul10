handle_rekey(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  tx_message->status = htons(STT_SUCCESS);
  KEY_Reload();
}
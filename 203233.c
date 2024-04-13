handle_make_step(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  LCL_MakeStep(0.0);
  tx_message->status = htons(STT_SUCCESS);
}
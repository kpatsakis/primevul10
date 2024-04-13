handle_manual_delete(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  int index;

  index = ntohl(rx_message->data.manual_delete.index);
  status = MNL_DeleteSample(index);
  if (!status) {
    tx_message->status = htons(STT_BADSAMPLE);
  } else {
    tx_message->status = htons(STT_SUCCESS);
  }
}  
handle_offline(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address, mask;
  UTI_IPNetworkToHost(&rx_message->data.offline.mask, &mask);
  UTI_IPNetworkToHost(&rx_message->data.offline.address, &address);
  status = NSR_TakeSourcesOffline(&mask, &address);
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
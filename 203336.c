handle_modify_maxdelay(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address;
  UTI_IPNetworkToHost(&rx_message->data.modify_maxdelay.address, &address);
  status = NSR_ModifyMaxdelay(&address,
                              UTI_FloatNetworkToHost(rx_message->data.modify_maxdelay.new_max_delay));
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
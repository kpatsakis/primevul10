handle_modify_maxdelayratio(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address;
  UTI_IPNetworkToHost(&rx_message->data.modify_maxdelayratio.address, &address);
  status = NSR_ModifyMaxdelayratio(&address,
                                   UTI_FloatNetworkToHost(rx_message->data.modify_maxdelayratio.new_max_delay_ratio));
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
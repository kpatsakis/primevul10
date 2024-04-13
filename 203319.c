handle_modify_maxdelaydevratio(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address;
  UTI_IPNetworkToHost(&rx_message->data.modify_maxdelaydevratio.address, &address);
  status = NSR_ModifyMaxdelaydevratio(&address,
                                   UTI_FloatNetworkToHost(rx_message->data.modify_maxdelaydevratio.new_max_delay_dev_ratio));
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
handle_modify_polltarget(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address;
  UTI_IPNetworkToHost(&rx_message->data.modify_polltarget.address, &address);
  status = NSR_ModifyPolltarget(&address,
                             ntohl(rx_message->data.modify_polltarget.new_poll_target));
  
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}
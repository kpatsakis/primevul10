handle_denyall(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  IPAddr ip;
  int subnet_bits;
  UTI_IPNetworkToHost(&rx_message->data.allow_deny.ip, &ip);
  subnet_bits = ntohl(rx_message->data.allow_deny.subnet_bits);
  if (NCR_AddAccessRestriction(&ip, subnet_bits, 0, 1)) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_BADSUBNET);
  }              
}
handle_cmdaccheck(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  IPAddr ip;
  UTI_IPNetworkToHost(&rx_message->data.ac_check.ip, &ip);
  if (CAM_CheckAccessRestriction(&ip)) {
    tx_message->status = htons(STT_ACCESSALLOWED);
  } else {
    tx_message->status = htons(STT_ACCESSDENIED);
  }
}
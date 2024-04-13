handle_local(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int on_off, stratum;
  on_off = ntohl(rx_message->data.local.on_off);
  if (on_off) {
    stratum = ntohl(rx_message->data.local.stratum);
    REF_EnableLocal(stratum);
  } else {
    REF_DisableLocal();
  }
  tx_message->status = htons(STT_SUCCESS);
}
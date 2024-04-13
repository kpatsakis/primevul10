static void send_request_chunk(void) {
  layoutProgress(_("Signing"), (data_total - data_left) * 1000 / data_total);
  msg_tx_request.has_data_length = true;
  msg_tx_request.data_length = data_left <= 1024 ? data_left : 1024;
  msg_write(MessageType_MessageType_EthereumTxRequest, &msg_tx_request);
}
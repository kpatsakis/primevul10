handle_n_sources(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int n_sources;
  n_sources = SRC_ReadNumberOfSources();
  tx_message->status = htons(STT_SUCCESS);
  tx_message->reply = htons(RPY_N_SOURCES);
  tx_message->data.n_sources.n_sources = htonl(n_sources);
}
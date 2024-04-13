handle_reselect_distance(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  double dist;
  dist = UTI_FloatNetworkToHost(rx_message->data.reselect_distance.distance);
  SRC_SetReselectDistance(dist);
  tx_message->status = htons(STT_SUCCESS);
}
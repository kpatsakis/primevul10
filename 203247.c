check_rx_packet_auth(CMD_Request *packet, int packet_len)
{
  int pkt_len, auth_len;

  pkt_len = PKL_CommandLength(packet);
  auth_len = packet_len - pkt_len;

  return KEY_CheckAuth(KEY_GetCommandKey(), (unsigned char *)packet,
      pkt_len, ((unsigned char *)packet) + pkt_len, auth_len);
}
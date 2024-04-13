generate_tx_packet_auth(CMD_Reply *packet)
{
  int pkt_len;

  pkt_len = PKL_ReplyLength(packet);

  return KEY_GenerateAuth(KEY_GetCommandKey(), (unsigned char *)packet,
      pkt_len, ((unsigned char *)packet) + pkt_len, sizeof (packet->auth));
}
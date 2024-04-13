print_reply_packet(CMD_Reply *pkt)
{
  unsigned char *x;
  int i;
  x = (unsigned char *) pkt;
  for (i=0; i<sizeof(CMD_Reply); i++) {
    fprintf(stderr, "%02x ", x[i]);
    if (i%16 == 15) {
      fprintf(stderr, "\n");
    }
  }
  fprintf(stderr, "\n");
}
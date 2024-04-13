print_command_packet(CMD_Request *pkt, int length)
{
  unsigned char *x;
  int i;
  x = (unsigned char *) pkt;
  for (i=0; i<length; i++) {
    fprintf(stderr, "%02x ", x[i]);
    if (i%16 == 15) {
      fprintf(stderr, "\n");
    }
  }
  fprintf(stderr, "\n");
}
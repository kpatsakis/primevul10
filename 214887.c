static void reduceBDbits(uint32_t *bd, unsigned int len) {
  int mask = 0;
  int shift = 0;
  unsigned int i = 0;

  for(i = 0; i < len; i++)
    mask = mask | bd[i];

  mask = mask >> 8;
  for(i = 0; i < 24 && mask; i++) {
    mask = mask >> 1;
    if (mask == 0) {
      shift = i+1;
      break;
    }
  }

  for(i = 0; i < len; i++)
    bd[i] = bd[i] >> shift;
}
void _MD5_encode(unsigned char *pout, unsigned int *pin, unsigned int len) {
   unsigned int i, j;

   for (i = 0, j = 0; j < len; i++, j += 4) {
      pout[j] = (unsigned char) (pin[i] & 0x0ff);
      pout[j + 1] = (unsigned char) ((pin[i] >> 8) & 0x0ff);
      pout[j + 2] = (unsigned char) ((pin[i] >> 16) & 0x0ff);
      pout[j + 3] = (unsigned char) ((pin[i] >> 24) & 0x0ff);
   }
}
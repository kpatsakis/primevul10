void _MD5_decode(unsigned int *pout, unsigned char *pin, unsigned int len) {
   unsigned int i, j;

   for (i = 0, j = 0; j < len; i++, j += 4)
      pout[i] = ((unsigned int) pin[j]) | (((unsigned int) pin[j + 1]) << 8) | (((unsigned int) pin[j + 2])
              << 16) | (((unsigned int)
              pin[j + 3]) << 24);
}
static void ito64(char *s, long v, int n) {
  while (--n >= 0) {
    *s++ = itoa64[v&0x3f];
    v >>= 6;
  }
}
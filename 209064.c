void base64_encode(unsigned char *s, unsigned char *d, int size) {
   unsigned int t, pad;
   unsigned char *p;

   pad = 3 - strlen((char *) s) % 3;
   if (pad == 3)
      pad = 0;
   p = d;
   while (*s) {
      t = (*s++) << 16;
      if (*s)
         t |= (*s++) << 8;
      if (*s)
         t |= (*s++) << 0;

      *(d + 3) = map[t & 63];
      t >>= 6;
      *(d + 2) = map[t & 63];
      t >>= 6;
      *(d + 1) = map[t & 63];
      t >>= 6;
      *(d + 0) = map[t & 63];

      d += 4;
      if (d - p >= size - 3)
         return;
   }
   *d = 0;
   while (pad--)
      *(--d) = '=';
}
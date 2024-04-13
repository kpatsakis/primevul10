void base64_bufenc(unsigned char *s, int len, char *d) {
   unsigned int t, pad;
   int i;

   pad = 3 - len % 3;
   if (pad == 3)
      pad = 0;
   for (i = 0; i < len;) {
      t = s[i++] << 16;
      if (i < len)
         t |= s[i++] << 8;
      if (i < len)
         t |= s[i++] << 0;

      *(d + 3) = map[t & 63];
      t >>= 6;
      *(d + 2) = map[t & 63];
      t >>= 6;
      *(d + 1) = map[t & 63];
      t >>= 6;
      *(d + 0) = map[t & 63];

      d += 4;
   }
   *d = 0;
   while (pad--)
      *(--d) = '=';
}
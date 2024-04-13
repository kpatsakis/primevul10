void base64_decode(char *s, char *d) {
   unsigned int t;

   while (s && *s) {
      t = cind(*s) << 18;
      s++;
      t |= cind(*s) << 12;
      s++;
      t |= cind(*s) << 6;
      s++;
      t |= cind(*s) << 0;
      s++;

      *(d + 2) = (char) (t & 0xFF);
      t >>= 8;
      *(d + 1) = (char) (t & 0xFF);
      t >>= 8;
      *d = (char) (t & 0xFF);

      d += 3;
   }
   *d = 0;
}
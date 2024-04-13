void MD5_checksum(const void *pdata, unsigned int len, unsigned char digest[16]) {
   MD5_CONTEXT ctx;
   unsigned char bits[8];
   unsigned int i, padlen;

   /* to allow multithreading we have to locate the padding memory here */
   unsigned char PADDING[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0
   };

   memset(&ctx, 0, sizeof(MD5_CONTEXT));
   ctx.count[0] = ctx.count[1] = 0;

   /* load magic initialization constants */
   ctx.state[0] = 0x67452301;
   ctx.state[1] = 0xefcdab89;
   ctx.state[2] = 0x98badcfe;
   ctx.state[3] = 0x10325476;

   _MD5_update(&ctx, pdata, len);

   // save number of bits
   _MD5_encode(bits, ctx.count, 8);

   // pad out to 56 mod 64
   i = (unsigned int) ((ctx.count[0] >> 3) & 0x3f);
   padlen = (i < 56) ? (56 - i) : (120 - i);
   _MD5_update(&ctx, PADDING, padlen);

   // append length (before padding)
   _MD5_update(&ctx, bits, 8);

   // store state in digest
   _MD5_encode(digest, ctx.state, 16);
}
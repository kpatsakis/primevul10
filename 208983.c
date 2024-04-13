void _MD5_update(MD5_CONTEXT *pctx, const void *pdata, unsigned int len) {
   unsigned char *pin;
   unsigned int i, index, partlen;

   pin = (unsigned char *) pdata;

   // compute number of bytes mod 64
   index = (unsigned int) ((pctx->count[0] >> 3) & 0x3F);

   // update number of bits
   if ((pctx->count[0] += ((unsigned int) len << 3)) < ((unsigned int) len << 3))
      pctx->count[1]++;
   pctx->count[1] += ((unsigned int) len >> 29);

   partlen = 64 - index;

   // transform as many times as possible.
   if (len >= partlen) {
      memcpy(&pctx->buffer[index], pin, partlen);
      _MD5_transform(pctx->state, pctx->buffer);

      for (i = partlen; i + 63 < len; i += 64)
         _MD5_transform(pctx->state, &pin[i]);

      index = 0;
   } else
      i = 0;

   /* buffer remaining input */
   memcpy(&pctx->buffer[index], &pin[i], len - i);
}
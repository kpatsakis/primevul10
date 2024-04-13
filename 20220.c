gx_ht_threshold_row_bit(byte *contone,  byte *threshold_strip,  int contone_stride,
                  byte *halftone, int dithered_stride, int width,
                  int num_rows, int offset_bits)
{
#ifndef HAVE_SSE2
    int k, j;
    byte *contone_ptr;
    byte *thresh_ptr;
    byte *halftone_ptr;
    byte bit_init;

    /* For the moment just do a very slow compare until we get
       get this working.  This could use some serious optimization */
    width -= offset_bits;
    for (j = 0; j < num_rows; j++) {
        byte h;
        contone_ptr = contone;
        thresh_ptr = threshold_strip + contone_stride * j;
        halftone_ptr = halftone + dithered_stride * j;
        /* First get the left remainder portion.  Put into MSBs of first byte */
        bit_init = 0x80;
        h = 0;
        k = offset_bits;
        if (k > 0) {
            do {
                if (*contone_ptr++ < *thresh_ptr++) {
                    h |=  bit_init;
                }
                bit_init >>= 1;
                if (bit_init == 0) {
                    bit_init = 0x80;
                    *halftone_ptr++ = h;
                    h = 0;
                }
                k--;
            } while (k > 0);
            bit_init = 0x80;
            *halftone_ptr++ = h;
            h = 0;
            if (offset_bits < 8)
                *halftone_ptr++ = 0;
        }
        /* Now get the rest, which will be 16 bit aligned. */
        k = width;
        if (k > 0) {
            do {
                if (*contone_ptr++ < *thresh_ptr++) {
                    h |=  bit_init;
                }
                bit_init >>= 1;
                if (bit_init == 0) {
                    bit_init = 0x80;
                    *halftone_ptr++ = h;
                    h = 0;
                }
                k--;
            } while (k > 0);
            if (bit_init != 0x80) {
                *halftone_ptr++ = h;
            }
            if ((width & 15) < 8)
                *halftone_ptr++ = 0;
        }
    }
#else
    byte *contone_ptr;
    byte *thresh_ptr;
    byte *halftone_ptr;
    int num_tiles = (width - offset_bits + 15)>>4;
    int k, j;

    for (j = 0; j < num_rows; j++) {
        /* contone and thresh_ptr are 128 bit aligned.  We do need to do this in
           two steps to ensure that we pack the bits in an aligned fashion
           into halftone_ptr.  */
        contone_ptr = contone;
        thresh_ptr = threshold_strip + contone_stride * j;
        halftone_ptr = halftone + dithered_stride * j;
        if (offset_bits > 0) {
            /* Since we allowed for 16 bits in our left remainder
               we can go directly in to the destination.  threshold_16_SSE
               requires 128 bit alignment.  contone_ptr and thresh_ptr
               are set up so that after we move in by offset_bits elements
               then we are 128 bit aligned.  */
            threshold_16_SSE_unaligned(contone_ptr, thresh_ptr,
                                       halftone_ptr);
            halftone_ptr += 2;
            thresh_ptr += offset_bits;
            contone_ptr += offset_bits;
        }
        /* Now we should have 128 bit aligned with our input data. Iterate
           over sets of 16 going directly into our HT buffer.  Sources and
           halftone_ptr buffers should be padded to allow 15 bit overrun */
        for (k = 0; k < num_tiles; k++) {
            threshold_16_SSE(contone_ptr, thresh_ptr, halftone_ptr);
            thresh_ptr += 16;
            contone_ptr += 16;
            halftone_ptr += 2;
        }
    }
#endif
}
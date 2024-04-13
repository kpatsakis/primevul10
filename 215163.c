static int ac_uncompress(EXRContext *s, GetByteContext *gb, float *block)
{
    int ret = 0, n = 1;

    while (n < 64) {
        uint16_t val = bytestream2_get_ne16(gb);

        if (val == 0xff00) {
            n = 64;
        } else if ((val >> 8) == 0xff) {
            n += val & 0xff;
        } else {
            ret = n;
            block[ff_zigzag_direct[n]] = av_int2float(half2float(val,
                                                      s->mantissatable,
                                                      s->exponenttable,
                                                      s->offsettable));
            n++;
        }
    }

    return ret;
}
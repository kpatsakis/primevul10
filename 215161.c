static int huf_decode(VLC *vlc, GetByteContext *gb, int nbits, int run_sym,
                      int no, uint16_t *out)
{
    GetBitContext gbit;
    int oe = 0;

    init_get_bits(&gbit, gb->buffer, nbits);
    while (get_bits_left(&gbit) > 0 && oe < no) {
        uint16_t x = get_vlc2(&gbit, vlc->table, 12, 3);

        if (x == run_sym) {
            int run = get_bits(&gbit, 8);
            uint16_t fill;

            if (oe == 0 || oe + run > no)
                return AVERROR_INVALIDDATA;

            fill = out[oe - 1];

            while (run-- > 0)
                out[oe++] = fill;
        } else {
            out[oe++] = x;
        }
    }

    return 0;
}
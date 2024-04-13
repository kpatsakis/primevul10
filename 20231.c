threshold_16_bit(byte *contone_ptr, byte *thresh_ptr, byte *ht_data)
{
    int j;

    for (j = 2; j > 0; j--) {
        byte h = 0;
        byte bit_init = 0x80;
        do {
            if (*contone_ptr++ < *thresh_ptr++) {
                h |=  bit_init;
            }
            bit_init >>= 1;
        } while (bit_init != 0);
        *ht_data++ = h;
    }
}
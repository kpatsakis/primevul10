static void dct_inverse(float *block)
{
    for (int i = 0; i < 8; i++)
        idct_1d(block + i, 8);

    for (int i = 0; i < 8; i++) {
        idct_1d(block, 1);
        block += 8;
    }
}
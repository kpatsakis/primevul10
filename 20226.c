threshold_16_SSE(byte *contone_ptr, byte *thresh_ptr, byte *ht_data)
{
    __m128i input1;
    __m128i input2;
    register int result_int;
    const unsigned int mask1 = 0x80808080;
    __m128i sign_fix = _mm_set_epi32(mask1, mask1, mask1, mask1);

    /* Load */
    input1 = _mm_load_si128((const __m128i *)contone_ptr);
    input2 = _mm_load_si128((const __m128i *) thresh_ptr);
    /* Unsigned subtraction does Unsigned saturation so we
       have to use the signed operation */
    input1 = _mm_xor_si128(input1, sign_fix);
    input2 = _mm_xor_si128(input2, sign_fix);
    /* Subtract the two */
    input2 = _mm_subs_epi8(input1, input2);
    /* Grab the sign mask */
    result_int = _mm_movemask_epi8(input2);
    /* bit wise reversal on 16 bit word */
    ht_data[0] = bitreverse[(result_int & 0xff)];
    ht_data[1] = bitreverse[((result_int >> 8) & 0xff)];
}
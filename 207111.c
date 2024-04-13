static int test_bn2padded(void)
{
    uint8_t zeros[256], out[256], reference[128];
    size_t bytes;
    BIGNUM *n;
    int st = 0;

    /* Test edge case at 0. */
    if (!TEST_ptr((n = BN_new())))
        goto err;
    if (!TEST_int_eq(BN_bn2binpad(n, NULL, 0), 0))
        goto err;
    memset(out, -1, sizeof(out));
    if (!TEST_int_eq(BN_bn2binpad(n, out, sizeof(out)), sizeof(out)))
        goto err;
    memset(zeros, 0, sizeof(zeros));
    if (!TEST_mem_eq(zeros, sizeof(zeros), out, sizeof(out)))
        goto err;

    /* Test a random numbers at various byte lengths. */
    for (bytes = 128 - 7; bytes <= 128; bytes++) {
# define TOP_BIT_ON 0
# define BOTTOM_BIT_NOTOUCH 0
        if (!TEST_true(BN_rand(n, bytes * 8, TOP_BIT_ON, BOTTOM_BIT_NOTOUCH)))
            goto err;
        if (!TEST_int_eq(BN_num_bytes(n), bytes)
                || !TEST_int_eq(BN_bn2bin(n, reference), bytes))
            goto err;
        /* Empty buffer should fail. */
        if (!TEST_int_eq(BN_bn2binpad(n, NULL, 0), -1))
            goto err;
        /* One byte short should fail. */
        if (!TEST_int_eq(BN_bn2binpad(n, out, bytes - 1), -1))
            goto err;
        /* Exactly right size should encode. */
        if (!TEST_int_eq(BN_bn2binpad(n, out, bytes), bytes)
                || !TEST_mem_eq(out, bytes, reference, bytes))
            goto err;
        /* Pad up one byte extra. */
        if (!TEST_int_eq(BN_bn2binpad(n, out, bytes + 1), bytes + 1)
                || !TEST_mem_eq(out + 1, bytes, reference, bytes)
                || !TEST_mem_eq(out, 1, zeros, 1))
            goto err;
        /* Pad up to 256. */
        if (!TEST_int_eq(BN_bn2binpad(n, out, sizeof(out)), sizeof(out))
                || !TEST_mem_eq(out + sizeof(out) - bytes, bytes,
                                reference, bytes)
                || !TEST_mem_eq(out, sizeof(out) - bytes,
                                zeros, sizeof(out) - bytes))
            goto err;
    }

    st = 1;
 err:
    BN_free(n);
    return st;
}
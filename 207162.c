static int test_mpi(int i)
{
    uint8_t scratch[8];
    const MPITEST *test = &kMPITests[i];
    size_t mpi_len, mpi_len2;
    BIGNUM *bn = NULL;
    BIGNUM *bn2 = NULL;
    int st = 0;

    if (!TEST_ptr(bn = BN_new())
            || !TEST_true(BN_asc2bn(&bn, test->base10)))
        goto err;
    mpi_len = BN_bn2mpi(bn, NULL);
    if (!TEST_size_t_le(mpi_len, sizeof(scratch)))
        goto err;

    if (!TEST_size_t_eq(mpi_len2 = BN_bn2mpi(bn, scratch), mpi_len)
            || !TEST_mem_eq(test->mpi, test->mpi_len, scratch, mpi_len))
        goto err;

    if (!TEST_ptr(bn2 = BN_mpi2bn(scratch, mpi_len, NULL)))
        goto err;

    if (!TEST_BN_eq(bn, bn2)) {
        BN_free(bn2);
        goto err;
    }
    BN_free(bn2);

    st = 1;
 err:
    BN_free(bn);
    return st;
}
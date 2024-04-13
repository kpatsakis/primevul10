static int run_file_tests(int i)
{
    STANZA *s = NULL;
    char *testfile = test_get_argument(i);
    int c;

    if (!TEST_ptr(s = OPENSSL_zalloc(sizeof(*s))))
        return 0;
    if (!test_start_file(s, testfile)) {
        OPENSSL_free(s);
        return 0;
    }

    /* Read test file. */
    while (!BIO_eof(s->fp) && test_readstanza(s)) {
        if (s->numpairs == 0)
            continue;
        if (!file_test_run(s))
            s->errors++;
        s->numtests++;
        test_clearstanza(s);
    }
    test_end_file(s);
    c = s->errors;
    OPENSSL_free(s);

    return c == 0;
}
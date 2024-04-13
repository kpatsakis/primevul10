static int file_test_run(STANZA *s)
{
    static const FILETEST filetests[] = {
        {"Sum", file_sum},
        {"LShift1", file_lshift1},
        {"LShift", file_lshift},
        {"RShift", file_rshift},
        {"Square", file_square},
        {"Product", file_product},
        {"Quotient", file_quotient},
        {"ModMul", file_modmul},
        {"ModExp", file_modexp},
        {"Exp", file_exp},
        {"ModSqrt", file_modsqrt},
        {"GCD", file_gcd},
    };
    int numtests = OSSL_NELEM(filetests);
    const FILETEST *tp = filetests;

    for ( ; --numtests >= 0; tp++) {
        if (findattr(s, tp->name) != NULL) {
            if (!tp->func(s)) {
                TEST_info("%s:%d: Failed %s test",
                          s->test_file, s->start, tp->name);
                return 0;
            }
            return 1;
        }
    }
    TEST_info("%s:%d: Unknown test", s->test_file, s->start);
    return 0;
}
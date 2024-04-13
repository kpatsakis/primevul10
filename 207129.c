const OPTIONS *test_get_options(void)
{
    static const OPTIONS test_options[] = {
        OPT_TEST_OPTIONS_WITH_EXTRA_USAGE("[file...]\n"),
        { "stochastic", OPT_STOCHASTIC_TESTS, '-', "Run stochastic tests" },
        { OPT_HELP_STR, 1, '-',
          "file\tFile to run tests on. Normal tests are not run\n" },
        { NULL }
    };
    return test_options;
}
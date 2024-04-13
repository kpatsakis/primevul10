char *BN_options(void)
{
    static int init = 0;
    static char data[16];

    if (!init) {
        init++;
#ifdef BN_LLONG
        BIO_snprintf(data, sizeof data, "bn(%d,%d)",
                     (int)sizeof(BN_ULLONG) * 8, (int)sizeof(BN_ULONG) * 8);
#else
        BIO_snprintf(data, sizeof data, "bn(%d,%d)",
                     (int)sizeof(BN_ULONG) * 8, (int)sizeof(BN_ULONG) * 8);
#endif
    }
    return (data);
}
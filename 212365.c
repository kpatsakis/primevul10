static int ftypin(int size)
{
    enum {BUFSIZE = 40};
    char buf[BUFSIZE];
    uint32_t u32;

    buf[4] = 0;
    datain(buf, 4);
    u32 = u32in();

    if (mp4config.verbose.header)
        fprintf(stderr, "Brand:\t\t\t%s(version %d)\n", buf, u32);

    stringin(buf, BUFSIZE);

    if (mp4config.verbose.header)
        fprintf(stderr, "Compatible brands:\t%s\n", buf);

    return size;
}
static int hdlr1in(int size)
{
    uint8_t buf[5];

    buf[4] = 0;
    // version/flags
    u32in();
    // pre_defined
    u32in();
    // Component subtype
    datain(buf, 4);
    if (mp4config.verbose.header)
        fprintf(stderr, "*track media type: '%s': ", buf);
    if (memcmp("soun", buf, 4))
    {
        if (mp4config.verbose.header)
            fprintf(stderr, "unsupported, skipping\n");
        return ERR_UNSUPPORTED;
    }
    else
    {
        if (mp4config.verbose.header)
            fprintf(stderr, "OK\n");
    }
    // reserved
    u32in();
    u32in();
    u32in();
    // name
    // null terminate
    u8in();

    return size;
};
int mp4read_open(char *name)
{
    uint32_t atomsize;
    int ret;

    mp4read_close();

    g_fin = faad_fopen(name, "rb");
    if (!g_fin)
        return ERR_FAIL;

    if (mp4config.verbose.header)
        fprintf(stderr, "**** MP4 header ****\n");
    g_atom = g_head;
    atomsize = INT_MAX;
    if (parse(&atomsize) < 0)
        goto err;
    g_atom = g_moov;
    atomsize = INT_MAX;
    rewind(g_fin);
    if ((ret = parse(&atomsize)) < 0)
    {
        fprintf(stderr, "parse:%d\n", ret);
        goto err;
    }

    // alloc frame buffer
    mp4config.bitbuf.data = malloc(mp4config.frame.maxsize);

    if (!mp4config.bitbuf.data)
        goto err;

    if (mp4config.verbose.header)
    {
        mp4info();
        fprintf(stderr, "********************\n");
    }

    if (mp4config.verbose.tags)
    {
        rewind(g_fin);
        g_atom = g_meta1;
        atomsize = INT_MAX;
        ret = parse(&atomsize);
        if (ret < 0)
        {
            rewind(g_fin);
            g_atom = g_meta2;
            atomsize = INT_MAX;
            ret = parse(&atomsize);
        }
    }

    return ERR_OK;
err:
    mp4read_close();
    return ERR_FAIL;
}
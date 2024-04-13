int mp4read_frame(void)
{
    if (mp4config.frame.current >= mp4config.frame.ents)
        return ERR_FAIL;

    mp4config.bitbuf.size = mp4config.frame.data[mp4config.frame.current + 1]
        - mp4config.frame.data[mp4config.frame.current];

    if (fread(mp4config.bitbuf.data, 1, mp4config.bitbuf.size, g_fin)
        != mp4config.bitbuf.size)
    {
        fprintf(stderr, "can't read frame data(frame %d@0x%x)\n",
               mp4config.frame.current,
               mp4config.frame.data[mp4config.frame.current]);

        return ERR_FAIL;
    }

    mp4config.frame.current++;

    return ERR_OK;
}
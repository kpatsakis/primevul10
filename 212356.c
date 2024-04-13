int mp4read_seek(int framenum)
{
    if (framenum > mp4config.frame.ents)
        return ERR_FAIL;
    if (fseek(g_fin, mp4config.mdatofs + mp4config.frame.data[framenum], SEEK_SET))
        return ERR_FAIL;

    mp4config.frame.current = framenum;

    return ERR_OK;
}
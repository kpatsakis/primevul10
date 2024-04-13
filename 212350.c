static int stcoin(int size)
{
    // version/flags
    u32in();
    // Number of entries
    if (u32in() < 1)
        return ERR_FAIL;
    // first chunk offset
    mp4config.mdatofs = u32in();
    // ignore the rest

    return size;
}
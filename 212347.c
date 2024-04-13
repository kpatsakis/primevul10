static int stsdin(int size)
{
    // version/flags
    u32in();
    // Number of entries(one 'mp4a')
    if (u32in() != 1) //fixme: error handling
        return ERR_FAIL;

    return size;
};
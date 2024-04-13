static char *mp4time(time_t t)
{
    int y;

    // subtract some seconds from the start of 1904 to the start of 1970
    for (y = 1904; y < 1970; y++)
    {
        t -= 365 * SECSINDAY;
        if (!(y & 3))
            t -= SECSINDAY;
    }
    return ctime(&t);
}
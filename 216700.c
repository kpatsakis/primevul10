compute_wakeup_sigma(const struct timeval *delta)
{
    if (delta->tv_sec < 1)
    {
        /* if < 1 sec, fuzz = # of microseconds / 8 */
        return delta->tv_usec >> 3;
    }
    else
    {
        /* if < 10 minutes, fuzz = 13.1% of timeout */
        if (delta->tv_sec < 600)
        {
            return delta->tv_sec << 17;
        }
        else
        {
            return 120000000; /* if >= 10 minutes, fuzz = 2 minutes */
        }
    }
}
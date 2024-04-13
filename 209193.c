PollReset(PollData *pdata)
{
    NS_NONNULL_ASSERT(pdata != NULL);
    pdata->nfds = 0u;
    pdata->timeout.sec = TIME_T_MAX;
    pdata->timeout.usec = 0;
}
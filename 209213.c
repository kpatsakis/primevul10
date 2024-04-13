PollFree(PollData *pdata)
{
    NS_NONNULL_ASSERT(pdata != NULL);
    ns_free(pdata->pfds);
    memset(pdata, 0, sizeof(PollData));
}
PollCreate(PollData *pdata)
{
    NS_NONNULL_ASSERT(pdata != NULL);
    memset(pdata, 0, sizeof(PollData));
}
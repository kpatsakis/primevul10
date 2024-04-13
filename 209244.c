PollWait(const PollData *pdata, int timeout)
{
    int n;

    NS_NONNULL_ASSERT(pdata != NULL);

    do {
        n = ns_poll(pdata->pfds, pdata->nfds, timeout);
    } while (n < 0  && errno == NS_EINTR);

    if (n < 0) {
        Ns_Fatal("PollWait: ns_poll() failed: %s", ns_sockstrerror(ns_sockerrno));
    }
    return n;
}
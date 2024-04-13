PollSet(PollData *pdata, NS_SOCKET sock, short type, const Ns_Time *timeoutPtr)
{
    NS_NONNULL_ASSERT(pdata != NULL);
    /*
     * Grow the pfds array if necessary.
     */

    if (unlikely(pdata->nfds >= pdata->maxfds)) {
        pdata->maxfds += 100u;
        pdata->pfds = ns_realloc(pdata->pfds, pdata->maxfds * sizeof(struct pollfd));
    }

    /*
     * Set the next pollfd struct with this socket.
     */

    pdata->pfds[pdata->nfds].fd = sock;
    pdata->pfds[pdata->nfds].events = type;
    pdata->pfds[pdata->nfds].revents = 0;

    /*
     * Check for new minimum timeout.
     */

    if (timeoutPtr != NULL && Ns_DiffTime(timeoutPtr, &pdata->timeout, NULL) < 0) {
        pdata->timeout = *timeoutPtr;
    }

    return pdata->nfds++;
}
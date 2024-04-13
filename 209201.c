SockClose(Sock *sockPtr, int keep)
{
    NS_NONNULL_ASSERT(sockPtr != NULL);

    if (keep != 0) {
        bool driverKeep = DriverKeep(sockPtr);
        keep = (int)driverKeep;
    }
    if (keep == (int)NS_FALSE) {
        DriverClose(sockPtr);
    }
    Ns_MutexLock(&sockPtr->drvPtr->lock);
    sockPtr->keep = (bool)keep;
    Ns_MutexUnlock(&sockPtr->drvPtr->lock);

    /*
     * Unconditionally remove temporary file, connection thread
     * should take care about very large uploads.
     */

    if (sockPtr->tfile != NULL) {
        unlink(sockPtr->tfile);
        ns_free(sockPtr->tfile);
        sockPtr->tfile = NULL;

        if (sockPtr->tfd > 0) {
            /*
             * Close and reset fd. The fd should be > 0 unless we are in error
             * conditions.
             */
            (void) ns_close(sockPtr->tfd);
        }
        sockPtr->tfd = 0;

    } else if (sockPtr->tfd > 0) {
        /*
         * This must be a fd allocated via Ns_GetTemp();
         */
        Ns_ReleaseTemp(sockPtr->tfd);
        sockPtr->tfd = 0;
    }

#ifndef _WIN32
    /*
     * Un-map temp file used for spooled content.
     */
    if (sockPtr->taddr != NULL) {
        munmap(sockPtr->taddr, (size_t)sockPtr->tsize);
        sockPtr->taddr = NULL;
    }
#endif
}
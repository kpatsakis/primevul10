NsAsyncWrite(int fd, const char *buffer, size_t nbyte)
{
    Ns_ReturnCode returnCode = NS_OK;

    NS_NONNULL_ASSERT(buffer != NULL);

    /*
     * If the async writer has not started or is deactivated, behave like a
     * ns_write() command. If the ns_write() fails, we can't do much, since
     * the writing of an error message to the log might bring us into an
     * infinite loop. So we print simple to stderr.
     */
    if (asyncWriter == NULL || asyncWriter->firstPtr->stopped) {
        ssize_t written = ns_write(fd, buffer, nbyte);

        if (unlikely(written != (ssize_t)nbyte)) {
            int retries = 100;

            /*
             * Don't go into an infinite loop when multiple subsequent disk
             * write operations return 0 (maybe disk full).
             */
            returnCode = NS_ERROR;
            do {
                if (written < 0) {
                    fprintf(stderr, "error during async write (fd %d): %s\n",
                           fd, strerror(errno));
                    break;
                }
                /*
                 * All partial writes (written >= 0)
                 */
                WriteWarningRaw("partial write", fd, nbyte, written);
                nbyte -= (size_t)written;
                buffer += written;
                written = ns_write(fd, buffer, nbyte);
                if (written == (ssize_t)nbyte) {
                    returnCode = NS_OK;
                    break;
                }
            } while (retries-- > 0);
        }

    } else {
        SpoolerQueue         *queuePtr;
        bool                  trigger = NS_FALSE;
        const AsyncWriteData *wdPtr;
        AsyncWriteData       *newWdPtr;

        /*
         * Allocate a writer cmd and initialize it. In order to provide an
         * interface compatible to ns_write(), we copy the provided data,
         * such it can be freed by the caller. When we would give up the
         * interface, we could free the memory block after writing, and
         * save a malloc/free operation on the data.
         */
        newWdPtr = ns_calloc(1u, sizeof(AsyncWriteData));
        newWdPtr->fd = fd;
        newWdPtr->bufsize = nbyte;
        newWdPtr->data = ns_malloc(nbyte + 1u);
        memcpy(newWdPtr->data, buffer, newWdPtr->bufsize);
        newWdPtr->buf  = newWdPtr->data;
        newWdPtr->size = newWdPtr->bufsize;

        /*
         * Now add new writer socket to the writer thread's queue. In most
         * cases, the queue will be empty.
         */
        queuePtr = asyncWriter->firstPtr;
        assert(queuePtr != NULL);

        Ns_MutexLock(&queuePtr->lock);
        wdPtr = queuePtr->sockPtr;
        if (wdPtr != NULL) {
            newWdPtr->nextPtr = queuePtr->sockPtr;
            queuePtr->sockPtr = newWdPtr;
        } else {
            queuePtr->sockPtr = newWdPtr;
            trigger = NS_TRUE;
        }
        Ns_MutexUnlock(&queuePtr->lock);

        /*
         * Wake up writer thread if desired
         */
        if (trigger) {
            SockTrigger(queuePtr->pipe[1]);
        }
    }

    return returnCode;
}
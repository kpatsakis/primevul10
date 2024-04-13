RequestFree(Sock *sockPtr)
{
    Request *reqPtr;
    bool     keep;

    NS_NONNULL_ASSERT(sockPtr != NULL);

    reqPtr = sockPtr->reqPtr;
    assert(reqPtr != NULL);

    Ns_Log(DriverDebug, "=== RequestFree cleans %p (avail %" PRIuz " keep %d length %" PRIuz " contentLength %" PRIuz ")",
           (void *)reqPtr, reqPtr->avail, sockPtr->keep, reqPtr->length, reqPtr->contentLength);

    keep = (sockPtr->keep) && (reqPtr->avail > reqPtr->contentLength);
    if (keep) {
        size_t      leftover = reqPtr->avail - reqPtr->contentLength;
        const char *offset   = reqPtr->buffer.string + ((size_t)reqPtr->buffer.length - leftover);

        Ns_Log(DriverDebug, "setting leftover to %" PRIuz " bytes", leftover);
        /*
         * Here it is safe to move the data in the buffer, although the
         * reqPtr->content might point to it, since we re-init the content. In
         * case the terminating null character was written to the end of the
         * previous buffer, we have to restore the first character.
         */
        memmove(reqPtr->buffer.string, offset, leftover);
        if (reqPtr->savedChar != '\0') {
            reqPtr->buffer.string[0] = reqPtr->savedChar;
        }
        Tcl_DStringSetLength(&reqPtr->buffer, (int)leftover);
        LogBuffer(DriverDebug, "KEEP BUFFER", reqPtr->buffer.string, leftover);
        reqPtr->leftover = leftover;
    } else {
        /*
         * Clean large buffers in order to avoid memory growth on huge
         * uploads (when maxupload is huge)
         */
        /*fprintf(stderr, "=== reuse buffer size %d avail %d dynamic %d\n",
                reqPtr->buffer.length, reqPtr->buffer.spaceAvl,
                reqPtr->buffer.string == reqPtr->buffer.staticSpace);*/
        if (Tcl_DStringLength(&reqPtr->buffer) > 65536) {
            Tcl_DStringFree(&reqPtr->buffer);
        } else {
            /*
             * Reuse buffer, but set length to 0.
             */
            Tcl_DStringSetLength(&reqPtr->buffer, 0);
        }
        reqPtr->leftover = 0u;
    }

    reqPtr->next           = NULL;
    reqPtr->content        = NULL;
    reqPtr->length         = 0u;
    reqPtr->contentLength  = 0u;

    reqPtr->expectedLength = 0u;
    reqPtr->chunkStartOff  = 0u;
    reqPtr->chunkWriteOff  = 0u;

    reqPtr->roff           = 0u;
    reqPtr->woff           = 0u;
    reqPtr->coff           = 0u;
    reqPtr->avail          = 0u;
    reqPtr->savedChar      = '\0';

    Ns_SetTrunc(reqPtr->headers, 0u);

    if (reqPtr->auth != NULL) {
        Ns_SetFree(reqPtr->auth);
        reqPtr->auth = NULL;
    }

    if (reqPtr->request.line != NULL) {
        Ns_Log(DriverDebug, "RequestFree calls Ns_ResetRequest on %p", (void*)&reqPtr->request);
        Ns_ResetRequest(&reqPtr->request);
    } else {
        Ns_Log(DriverDebug, "RequestFree does not call Ns_ResetRequest on %p", (void*)&reqPtr->request);
    }

    if (!keep) {
        /*
         * Push the reqPtr to the pool for reuse in other connections.
         */
        sockPtr->reqPtr = NULL;

        Ns_MutexLock(&reqLock);
        reqPtr->nextPtr = firstReqPtr;
        firstReqPtr = reqPtr;
        Ns_MutexUnlock(&reqLock);
    } else {
        /*
         * Keep the partly cleaned up reqPtr associated with the connection.
         */
        Ns_Log(DriverDebug, "=== KEEP request structure in sockPtr (don't push into the pool)");
    }
}
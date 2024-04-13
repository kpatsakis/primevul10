WriterSockRelease(WriterSock *wrSockPtr) {
    SpoolerQueue *queuePtr;

    NS_NONNULL_ASSERT(wrSockPtr != NULL);

    wrSockPtr->refCount --;

    Ns_Log(DriverDebug, "WriterSockRelease %p refCount %d keep %d",
           (void *)wrSockPtr, wrSockPtr->refCount, wrSockPtr->keep);

    if (wrSockPtr->refCount > 0) {
        return;
    }

    Ns_Log(DriverDebug,
           "Writer: closed sock %d, file fd %d, error %d/%d, "
           "sent=%" TCL_LL_MODIFIER "d, flags=%X",
           wrSockPtr->sockPtr->sock, wrSockPtr->fd,
           wrSockPtr->status, wrSockPtr->err,
           wrSockPtr->nsent, wrSockPtr->flags);

    NsPoolAddBytesSent(wrSockPtr->poolPtr, wrSockPtr->nsent);

    if (wrSockPtr->doStream != NS_WRITER_STREAM_NONE) {
        Conn *connPtr;

        NsWriterLock();
        connPtr = wrSockPtr->connPtr;
        if (connPtr != NULL && connPtr->strWriter != NULL) {
            connPtr->strWriter = NULL;
        }
        NsWriterUnlock();

        /*
         * In case, writer streams are activated for this wrSockPtr, make sure
         * to release the tmp file. See thread Naviserver Open Files on the
         * sourceforge mailing list (starting July 2019).
         */
        if (wrSockPtr->doStream == NS_WRITER_STREAM_FINISH) {
            Ns_ReleaseTemp(wrSockPtr->fd);
        }
    }

    /*
     * Remove the entry from the queue and decrement counter
     */
    queuePtr = wrSockPtr->queuePtr;
    if (queuePtr->curPtr == wrSockPtr) {
        queuePtr->curPtr = wrSockPtr->nextPtr;
        queuePtr->queuesize--;
    } else {
        WriterSock *curPtr, *lastPtr = queuePtr->curPtr;
        for (curPtr = (lastPtr != NULL) ? lastPtr->nextPtr : NULL;
             curPtr != NULL;
             lastPtr = curPtr, curPtr = curPtr->nextPtr
             ) {
            if (curPtr == wrSockPtr) {
                lastPtr->nextPtr = wrSockPtr->nextPtr;
                queuePtr->queuesize--;
                break;
            }
        }
    }

    if ((wrSockPtr->err != 0) || (wrSockPtr->status != SPOOLER_OK)) {
        int i;
        /*
         * Lookup the matching sockState from the spooler state. The array has
         * just 5 elements, on average, just 2 comparisons are needed (since
         * OK is at the end).
         */
        for (i = 0; i < Ns_NrElements(spoolerStateMap); i++) {
            if (spoolerStateMap[i].spoolerState == wrSockPtr->status) {
                SockError(wrSockPtr->sockPtr, spoolerStateMap[i].sockState, wrSockPtr->err);
                break;
            }
        }
        NsSockClose(wrSockPtr->sockPtr, (int)NS_FALSE);
    } else {
        NsSockClose(wrSockPtr->sockPtr, (int)wrSockPtr->keep);
    }
    if (wrSockPtr->clientData != NULL) {
        ns_free(wrSockPtr->clientData);
    }
    if (wrSockPtr->fd != NS_INVALID_FD) {
        if (wrSockPtr->doStream != NS_WRITER_STREAM_FINISH) {
            (void) ns_close(wrSockPtr->fd);
        }
        WriterSockFileVecCleanup(wrSockPtr);

    } else if (wrSockPtr->c.mem.bufs != NULL) {
        if (wrSockPtr->c.mem.fmap.addr != NULL) {
            NsMemUmap(&wrSockPtr->c.mem.fmap);

        } else {
            int i;
            for (i = 0; i < wrSockPtr->c.mem.nbufs; i++) {
                ns_free((char *)wrSockPtr->c.mem.bufs[i].iov_base);
            }
        }
        if (wrSockPtr->c.mem.bufs != wrSockPtr->c.mem.preallocated_bufs) {
            ns_free(wrSockPtr->c.mem.bufs);
        }
    }
    if (wrSockPtr->headerString != NULL) {
        ns_free(wrSockPtr->headerString);
    }

    ns_free(wrSockPtr);
}
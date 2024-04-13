WriterSockFileVecCleanup(WriterSock *wrSockPtr) {

    NS_NONNULL_ASSERT(wrSockPtr != NULL);

    if ( wrSockPtr->c.file.nbufs > 0) {
        int i;

        Ns_Log(DriverDebug, "WriterSockRelease nbufs %d", wrSockPtr->c.file.nbufs);

        for (i = 0; i < wrSockPtr->c.file.nbufs; i++) {
            /*
             * The fd of c.file.currentbuf is always the same as
             * wrSockPtr->fd and therefore already closed at this point.
             */
            if ( (i != wrSockPtr->c.file.currentbuf)
                 && (wrSockPtr->c.file.bufs[i].fd != NS_INVALID_FD) ) {

                Ns_Log(DriverDebug, "WriterSockRelease must close fd %d",
                       wrSockPtr->c.file.bufs[i].fd);
                ns_close(wrSockPtr->c.file.bufs[i].fd);
            }
        }
        ns_free(wrSockPtr->c.file.bufs);
    }
    ns_free(wrSockPtr->c.file.buf);
}
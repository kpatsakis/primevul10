WriterSockRequire(const Conn *connPtr) {
    WriterSock *wrSockPtr;

    NS_NONNULL_ASSERT(connPtr != NULL);

    NsWriterLock();
    wrSockPtr = (WriterSock *)connPtr->strWriter;
    if (wrSockPtr != NULL) {
        wrSockPtr->refCount ++;
    }
    NsWriterUnlock();
    return wrSockPtr;
}
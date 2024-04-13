NsWriterFinish(NsWriterSock *wrSockPtr) {
    WriterSock *writerSockPtr = (WriterSock *)wrSockPtr;

    NS_NONNULL_ASSERT(wrSockPtr != NULL);

    Ns_Log(DriverDebug, "NsWriterFinish: %p", (void *)writerSockPtr);
    writerSockPtr->doStream = NS_WRITER_STREAM_FINISH;
    SockTrigger(writerSockPtr->queuePtr->pipe[1]);
}
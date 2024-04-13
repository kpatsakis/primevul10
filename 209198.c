void NsWriterUnlock(void) {
    Ns_MutexUnlock(&writerlock);
}
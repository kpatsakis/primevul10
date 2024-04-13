static Jsi_RC MutexLock(Jsi_Interp *interp, Jsi_Mutex* mtx) {
    int timeout = mtx->lockTimeout;
    if (interp && timeout<0)
        timeout = interp->lockTimeout;
    if (timeout<=0)
        EnterCriticalSection(&mtx->mtx);
    else {
        int cnt = timeout;
        while (cnt-- >= 0) {
            if (TryEnterCriticalSection(&mtx->mtx))
                return JSI_OK;
            usleep(1000);
        }
        Jsi_LogError("lock timed out");
        if (interp)
            interp->threadErrCnt++;
        mtx->threadErrCnt++;
        return JSI_ERROR;
    }
    return JSI_OK;
}
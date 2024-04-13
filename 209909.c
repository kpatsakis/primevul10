static Jsi_RC MutexLock(Jsi_Interp *interp, Jsi_Mutex *mtx) {
    int timeout = mtx->lockTimeout;
    if (interp && timeout<0)
        timeout = interp->lockTimeout;
    if (timeout<=0)
        pthread_mutex_lock(&mtx->mtx);
    else {
        struct timespec ts;
        ts.tv_sec = timeout/1000;
        ts.tv_nsec = 1000 * (timeout%1000);
        int rc = pthread_mutex_timedlock(&mtx->mtx, &ts);
        if (rc != 0) {
            Jsi_LogError("lock timed out");
            if (interp)
                interp->threadErrCnt++;
            mtx->threadErrCnt++;
            return JSI_ERROR;
        }
    }
    return JSI_OK;
}
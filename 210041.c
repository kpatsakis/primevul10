Jsi_Mutex* Jsi_MutexNew(Jsi_Interp *interp, int timeout, int flags) {
     Jsi_Mutex *mtx = (Jsi_Mutex *)Jsi_Calloc(1,sizeof(Jsi_Mutex));
     mtx->lockTimeout = timeout;
     mtx->flags = flags;
     MutexInit(mtx);
     return mtx;
}
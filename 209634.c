static void MutexInit(Jsi_Mutex *mtx) {
    pthread_mutexattr_t Attr;
    pthread_mutexattr_init(&Attr);
    if (mtx->flags & JSI_MUTEX_RECURSIVE)
        pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mtx->mtx, &Attr);
}
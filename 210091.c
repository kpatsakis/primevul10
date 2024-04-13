void* Jsi_CurrentThread(void) {
#ifdef __WIN32
    return (void*)(uintptr_t)GetCurrentThreadId();
#else
    return (void*)pthread_self();
#endif
}
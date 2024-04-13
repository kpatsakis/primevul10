static void core_child_init(apr_pool_t *pchild, server_rec *s)
{
    apr_proc_t proc;
#if APR_HAS_THREADS
    int threaded_mpm;
    if (ap_mpm_query(AP_MPMQ_IS_THREADED, &threaded_mpm) == APR_SUCCESS
        && threaded_mpm)
    {
        apr_thread_mutex_create(&rng_mutex, APR_THREAD_MUTEX_DEFAULT, pchild);
    }
#endif
    /* The MPMs use plain fork() and not apr_proc_fork(), so we have to call
     * apr_random_after_fork() manually in the child
     */
    proc.pid = getpid();
    apr_random_after_fork(&proc);
}
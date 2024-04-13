static void assert_log_message(const char* format, ...)
{
    va_list  args;

    pthread_mutex_lock(&gAllocationsMutex);
    {
        const MallocDebug* current_dispatch = __libc_malloc_dispatch;
        __libc_malloc_dispatch = &__libc_malloc_default_dispatch;
        va_start(args, format);
        __libc_android_log_vprint(ANDROID_LOG_ERROR, "libc",
                                format, args);
        va_end(args);
        dump_stack_trace();
        if (gTrapOnError) {
            __builtin_trap();
        }
        __libc_malloc_dispatch = current_dispatch;
    }
    pthread_mutex_unlock(&gAllocationsMutex);
}
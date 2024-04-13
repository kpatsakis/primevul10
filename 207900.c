static void dump_stack_trace()
{
    intptr_t addrs[20];
    int c = get_backtrace(addrs, 20);
    char buf[16];
    char tmp[16*20];
    int i;

    tmp[0] = 0; // Need to initialize tmp[0] for the first strcat
    for (i=0 ; i<c; i++) {
        snprintf(buf, sizeof buf, "%2d: %08x\n", i, addrs[i]);
        strlcat(tmp, buf, sizeof tmp);
    }
    __libc_android_log_print(ANDROID_LOG_ERROR, "libc", "call stack:\n%s", tmp);
}
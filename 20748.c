FatalError(const char *f, ...)
{
    va_list args;
    va_list args2;
    static Bool beenhere = FALSE;

    if (beenhere)
        ErrorFSigSafe("\nFatalError re-entered, aborting\n");
    else
        ErrorFSigSafe("\nFatal server error:\n");

    va_start(args, f);

    /* Make a copy for OsVendorFatalError */
    va_copy(args2, args);

#ifdef __APPLE__
    {
        va_list apple_args;

        va_copy(apple_args, args);
        (void)vsnprintf(__crashreporter_info_buff__,
                        sizeof(__crashreporter_info_buff__), f, apple_args);
        va_end(apple_args);
    }
#endif
    VErrorFSigSafe(f, args);
    va_end(args);
    ErrorFSigSafe("\n");
    if (!beenhere)
        OsVendorFatalError(f, args2);
    va_end(args2);
    if (!beenhere) {
        beenhere = TRUE;
        AbortServer();
    }
    else
        OsAbort();
 /*NOTREACHED*/}
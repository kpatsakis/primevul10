AbortServer(void)
{
#ifdef XF86BIGFONT
    XF86BigfontCleanup();
#endif
    CloseWellKnownConnections();
    OsCleanup(TRUE);
    AbortDevices();
    AbortDDX(EXIT_ERR_ABORT);
    fflush(stderr);
    if (CoreDump)
        OsAbort();
    exit(1);
}
pnprintf(char *string, int size, const char *f, ...)
{
    int rc;
    va_list args;

    va_start(args, f);
    rc = vpnprintf(string, size, f, args);
    va_end(args);

    return rc;
}
int Jsi_Printf(Jsi_Interp *interp, Jsi_Channel chan, const char *fmt, ...)
{
    va_list va;
    int n;
    FILE *fp = (chan && chan->fp ? chan->fp : stdout);
    va_start(va,fmt);
    n = vfprintf(fp, fmt, va);
    va_end(va);
    return n;
}
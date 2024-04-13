Jsi_RC Jsi_LogMsg(Jsi_Interp *interp, uint code, const char *format,...) {
    va_list va;
    va_start (va, format);
    const char *mt = (code <= JSI__LOGLAST ? jsi_LogCodes[code] : "");
    fputs(mt, stderr);
    vfprintf(stderr, format, va);
    fputs("\n", stderr);
    va_end(va);
    return JSI_ERROR;
}
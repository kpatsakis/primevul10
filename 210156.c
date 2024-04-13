Jsi_RC Jsi_JSONParseFmt(Jsi_Interp *interp, Jsi_Value **ret, const char *fmt, ...) 
{
    va_list argList;
    uint n;
    char buf[JSI_BUFSIZ], *cp = buf;
    Jsi_DString dStr = {};
    va_start(argList, fmt);
    n = vsnprintf(buf, sizeof(buf), fmt, argList);
    if (n>JSI_MAX_ALLOC_BUF) {
        Jsi_LogError("Jsi_JSONParseFmt error: rc = %d", n);
        va_end(argList);
        return JSI_ERROR;
    }
    if (n >= sizeof(buf)) {
        uint m;
        Jsi_DSSetLength(&dStr, n+1);
        m = vsnprintf(Jsi_DSValue(&dStr), n+1, fmt, argList);
        assert(m == n);
        JSI_NOWARN(m);
        cp = Jsi_DSValue(&dStr);
    }
    va_end(argList);
    return Jsi_JSONParse(interp, cp, ret, 0);
}
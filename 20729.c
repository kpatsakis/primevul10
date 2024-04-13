VErrorF(const char *f, va_list args)
{
#ifdef DDXOSVERRORF
    if (OsVendorVErrorFProc)
        OsVendorVErrorFProc(f, args);
    else
        LogVWrite(-1, f, args);
#else
    LogVWrite(-1, f, args);
#endif
}
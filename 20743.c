VErrorFSigSafe(const char *f, va_list args)
{
    LogVMessageVerbSigSafe(X_ERROR, -1, f, args);
}
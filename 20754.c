LogVWrite(int verb, const char *f, va_list args)
{
    return LogVMessageVerb(X_NONE, verb, f, args);
}
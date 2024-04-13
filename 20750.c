LogMessageVerbSigSafe(MessageType type, int verb, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    LogVMessageVerbSigSafe(type, verb, format, ap);
    va_end(ap);
}
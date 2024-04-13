LogVMessageVerb(MessageType type, int verb, const char *format, va_list args)
{
    const char *type_str;
    char buf[1024];
    const size_t size = sizeof(buf);
    Bool newline;
    size_t len = 0;

    if (inSignalContext) {
        LogVMessageVerbSigSafe(type, verb, format, args);
        return;
    }

    type_str = LogMessageTypeVerbString(type, verb);
    if (!type_str)
        return;

    /* if type_str is not "", prepend it and ' ', to message */
    if (type_str[0] != '\0')
        len += Xscnprintf(&buf[len], size - len, "%s ", type_str);

    if (size - len > 1)
        len += Xvscnprintf(&buf[len], size - len, format, args);

    /* Force '\n' at end of truncated line */
    if (size - len == 1)
        buf[len - 1] = '\n';

    newline = (buf[len - 1] == '\n');
    LogSWrite(verb, buf, len, newline);
}
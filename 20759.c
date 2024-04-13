LogVMessageVerbSigSafe(MessageType type, int verb, const char *format, va_list args)
{
    const char *type_str;
    char buf[1024];
    int len;
    Bool newline;

    type_str = LogMessageTypeVerbString(type, verb);
    if (!type_str)
        return;

    /* if type_str is not "", prepend it and ' ', to message */
    if (type_str[0] != '\0') {
        LogSWrite(verb, type_str, strlen_sigsafe(type_str), FALSE);
        LogSWrite(verb, " ", 1, FALSE);
    }

    len = vpnprintf(buf, sizeof(buf), format, args);

    /* Force '\n' at end of truncated line */
    if (sizeof(buf) - len == 1)
        buf[len - 1] = '\n';

    newline = (len > 0 && buf[len - 1] == '\n');
    LogSWrite(verb, buf, len, newline);
}
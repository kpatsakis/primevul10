LogVHdrMessageVerb(MessageType type, int verb, const char *msg_format,
                   va_list msg_args, const char *hdr_format, va_list hdr_args)
{
    const char *type_str;
    char buf[1024];
    const size_t size = sizeof(buf);
    Bool newline;
    size_t len = 0;
    int (*vprintf_func)(char *, int, const char* _X_RESTRICT_KYWD f, va_list args)
            _X_ATTRIBUTE_PRINTF(3, 0);
    int (*printf_func)(char *, int, const char* _X_RESTRICT_KYWD f, ...)
            _X_ATTRIBUTE_PRINTF(3, 4);

    type_str = LogMessageTypeVerbString(type, verb);
    if (!type_str)
        return;

    if (inSignalContext) {
        vprintf_func = vpnprintf;
        printf_func = pnprintf;
    } else {
        vprintf_func = Xvscnprintf;
        printf_func = Xscnprintf;
    }

    /* if type_str is not "", prepend it and ' ', to message */
    if (type_str[0] != '\0')
        len += printf_func(&buf[len], size - len, "%s ", type_str);

    if (hdr_format && size - len > 1)
        len += vprintf_func(&buf[len], size - len, hdr_format, hdr_args);

    if (msg_format && size - len > 1)
        len += vprintf_func(&buf[len], size - len, msg_format, msg_args);

    /* Force '\n' at end of truncated line */
    if (size - len == 1)
        buf[len - 1] = '\n';

    newline = (buf[len - 1] == '\n');
    LogSWrite(verb, buf, len, newline);
}
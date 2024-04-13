LogHdrMessageVerb(MessageType type, int verb, const char *msg_format,
                  va_list msg_args, const char *hdr_format, ...)
{
    va_list hdr_args;

    va_start(hdr_args, hdr_format);
    LogVHdrMessageVerb(type, verb, msg_format, msg_args, hdr_format, hdr_args);
    va_end(hdr_args);
}
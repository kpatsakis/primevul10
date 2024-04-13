LogHdrMessage(MessageType type, const char *msg_format, va_list msg_args,
              const char *hdr_format, ...)
{
    va_list hdr_args;

    va_start(hdr_args, hdr_format);
    LogVHdrMessageVerb(type, 1, msg_format, msg_args, hdr_format, hdr_args);
    va_end(hdr_args);
}
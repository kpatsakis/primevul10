LogMessageTypeVerbString(MessageType type, int verb)
{
    if (type == X_ERROR)
        verb = 0;

    if (logVerbosity < verb && logFileVerbosity < verb)
        return NULL;

    switch (type) {
    case X_PROBED:
        return X_PROBE_STRING;
    case X_CONFIG:
        return X_CONFIG_STRING;
    case X_DEFAULT:
        return X_DEFAULT_STRING;
    case X_CMDLINE:
        return X_CMDLINE_STRING;
    case X_NOTICE:
        return X_NOTICE_STRING;
    case X_ERROR:
        return X_ERROR_STRING;
    case X_WARNING:
        return X_WARNING_STRING;
    case X_INFO:
        return X_INFO_STRING;
    case X_NOT_IMPLEMENTED:
        return X_NOT_IMPLEMENTED_STRING;
    case X_UNKNOWN:
        return X_UNKNOWN_STRING;
    case X_NONE:
        return X_NONE_STRING;
    case X_DEBUG:
        return X_DEBUG_STRING;
    default:
        return X_UNKNOWN_STRING;
    }
}
LogSetParameter(LogParameter param, int value)
{
    switch (param) {
    case XLOG_FLUSH:
        logFlush = value ? TRUE : FALSE;
        return TRUE;
    case XLOG_SYNC:
        logSync = value ? TRUE : FALSE;
        return TRUE;
    case XLOG_VERBOSITY:
        logVerbosity = value;
        return TRUE;
    case XLOG_FILE_VERBOSITY:
        logFileVerbosity = value;
        return TRUE;
    default:
        return FALSE;
    }
}
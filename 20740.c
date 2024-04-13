LogSWrite(int verb, const char *buf, size_t len, Bool end_line)
{
    static Bool newline = TRUE;
    int ret;

    if (verb < 0 || logVerbosity >= verb)
        ret = write(2, buf, len);

    if (verb < 0 || logFileVerbosity >= verb) {
        if (inSignalContext && logFileFd >= 0) {
            ret = write(logFileFd, buf, len);
#ifndef WIN32
            if (logFlush && logSync)
                fsync(logFileFd);
#endif
        }
        else if (!inSignalContext && logFile) {
            if (newline)
                fprintf(logFile, "[%10.3f] ", GetTimeInMillis() / 1000.0);
            newline = end_line;
            fwrite(buf, len, 1, logFile);
            if (logFlush) {
                fflush(logFile);
#ifndef WIN32
                if (logSync)
                    fsync(fileno(logFile));
#endif
            }
        }
        else if (!inSignalContext && needBuffer) {
            if (len > bufferUnused) {
                bufferSize += 1024;
                bufferUnused += 1024;
                saveBuffer = realloc(saveBuffer, bufferSize);
                if (!saveBuffer)
                    FatalError("realloc() failed while saving log messages\n");
            }
            bufferUnused -= len;
            memcpy(saveBuffer + bufferPos, buf, len);
            bufferPos += len;
        }
    }

    /* There's no place to log an error message if the log write
     * fails...
     */
    (void) ret;
}
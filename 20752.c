LogInit(const char *fname, const char *backup)
{
    char *logFileName = NULL;

    if (fname && *fname) {
        if (displayfd != -1) {
            /* Display isn't set yet, so we can't use it in filenames yet. */
            char pidstring[32];
            snprintf(pidstring, sizeof(pidstring), "pid-%ld",
                     (unsigned long) getpid());
            logFileName = LogFilePrep(fname, backup, pidstring);
            saved_log_tempname = logFileName;

            /* Save the patterns for use when the display is named. */
            saved_log_fname = strdup(fname);
            if (backup == NULL)
                saved_log_backup = NULL;
            else
                saved_log_backup = strdup(backup);
        } else
            logFileName = LogFilePrep(fname, backup, display);
        if ((logFile = fopen(logFileName, "w")) == NULL)
            FatalError("Cannot open log file \"%s\"\n", logFileName);
        setvbuf(logFile, NULL, _IONBF, 0);

        logFileFd = fileno(logFile);

        /* Flush saved log information. */
        if (saveBuffer && bufferSize > 0) {
            fwrite(saveBuffer, bufferPos, 1, logFile);
            fflush(logFile);
#ifndef WIN32
            fsync(fileno(logFile));
#endif
        }
    }

    /*
     * Unconditionally free the buffer, and flag that the buffer is no longer
     * needed.
     */
    if (saveBuffer && bufferSize > 0) {
        free(saveBuffer);
        saveBuffer = NULL;
        bufferSize = 0;
    }
    needBuffer = FALSE;

    return logFileName;
}
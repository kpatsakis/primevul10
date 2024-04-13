LogSetDisplay(void)
{
    if (saved_log_fname && strstr(saved_log_fname, "%s")) {
        char *logFileName;

        logFileName = LogFilePrep(saved_log_fname, saved_log_backup, display);

        if (rename(saved_log_tempname, logFileName) == 0) {
            LogMessageVerb(X_PROBED, 0,
                           "Log file renamed from \"%s\" to \"%s\"\n",
                           saved_log_tempname, logFileName);

            if (strlen(saved_log_tempname) >= strlen(logFileName))
                strncpy(saved_log_tempname, logFileName,
                        strlen(saved_log_tempname));
        }
        else {
            ErrorF("Failed to rename log file \"%s\" to \"%s\": %s\n",
                   saved_log_tempname, logFileName, strerror(errno));
        }

        /* free newly allocated string - can't free old one since existing
           pointers to it may exist in DDX callers. */
        free(logFileName);
        free(saved_log_fname);
        free(saved_log_backup);
    }
}
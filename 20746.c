LogClose(enum ExitCode error)
{
    if (logFile) {
        int msgtype = (error == EXIT_NO_ERROR) ? X_INFO : X_ERROR;
        LogMessageVerbSigSafe(msgtype, -1,
                "Server terminated %s (%d). Closing log file.\n",
                (error == EXIT_NO_ERROR) ? "successfully" : "with error",
                error);
        fclose(logFile);
        logFile = NULL;
        logFileFd = -1;
    }
}
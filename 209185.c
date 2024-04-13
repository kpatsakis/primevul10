LogBuffer(Ns_LogSeverity severity, const char *msg, const char *buffer, size_t len)
{
    Tcl_DString ds;

    NS_NONNULL_ASSERT(msg != NULL);
    NS_NONNULL_ASSERT(buffer != NULL);

    if (Ns_LogSeverityEnabled(severity)) {

        Tcl_DStringInit(&ds);
        Tcl_DStringAppend(&ds, msg, -1);
        Tcl_DStringAppend(&ds, ": ", 2);
        (void)Ns_DStringAppendPrintable(&ds, NS_FALSE, buffer, len);

        Ns_Log(severity, "%s", ds.string);
        Tcl_DStringFree(&ds);
    }
}
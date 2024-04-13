AsyncLogfileWriteObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int               result = TCL_OK, binary = (int)NS_FALSE, sanitize;
    Tcl_Obj          *stringObj;
    int               fd = 0;
    Ns_ObjvValueRange fd_range = {0, INT_MAX};
    Ns_ObjvValueRange sanitize_range = {0, 2};
    Ns_ObjvSpec opts[] = {
        {"-binary",    Ns_ObjvBool, &binary,   INT2PTR(NS_TRUE)},
        {"-sanitize",  Ns_ObjvInt,  &sanitize, &sanitize_range},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec args[] = {
        {"fd",     Ns_ObjvInt, &fd,        &fd_range},
        {"buffer", Ns_ObjvObj, &stringObj, NULL},
        {NULL, NULL, NULL, NULL}
    };

    /*
     * Take the config value as default for "-sanitize", but let the used
     * override it on a per-case basis.
     */
    sanitize = nsconf.sanitize_logfiles;

    if (unlikely(Ns_ParseObjv(opts, args, interp, 2, objc, objv) != NS_OK)) {
        result = TCL_ERROR;

    } else {
        const char   *buffer;
        int           length;
        Ns_ReturnCode rc;

        if (binary == (int)NS_TRUE || NsTclObjIsByteArray(stringObj)) {
            buffer = (const char *) Tcl_GetByteArrayFromObj(stringObj, &length);
        } else {
            buffer = Tcl_GetStringFromObj(stringObj, &length);
        }
        if (length > 0) {
            if (sanitize > 0) {
                Tcl_DString ds;
                bool        lastCharNewline = (buffer[length-1] == '\n');

                Tcl_DStringInit(&ds);
                if (lastCharNewline) {
                    length --;
                }
                Ns_DStringAppendPrintable(&ds, sanitize == 2, buffer, (size_t)length);
                if (lastCharNewline) {
                    Tcl_DStringAppend(&ds, "\n", 1);
                }
                rc = NsAsyncWrite(fd, ds.string, (size_t)ds.length);
                Tcl_DStringFree(&ds);

            } else {
                rc = NsAsyncWrite(fd, buffer, (size_t)length);
            }

            if (rc != NS_OK) {
                Ns_TclPrintfResult(interp, "ns_asynclogfile: error during write operation on fd %d: %s",
                                   fd, Tcl_PosixError(interp));
                result = TCL_ERROR;
            }
        } else {
            result = TCL_OK;
        }
    }
    return result;
}
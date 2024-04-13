WriterSubmitFileObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int         result = TCL_OK;
    Ns_Conn    *conn;
    char       *fileNameString;
    int         headers = 0;
    Tcl_WideInt offset = 0, size = 0;
    Ns_ObjvValueRange offsetRange = {0, LLONG_MAX};
    Ns_ObjvValueRange sizeRange = {1, LLONG_MAX};
    Ns_ObjvSpec lopts[] = {
        {"-headers",  Ns_ObjvBool,    &headers, INT2PTR(NS_TRUE)},
        {"-offset",   Ns_ObjvMemUnit, &offset,  &offsetRange},
        {"-size",     Ns_ObjvMemUnit, &size,    &sizeRange},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec args[] = {
        {"file",      Ns_ObjvString, &fileNameString, NULL},
        {NULL, NULL, NULL, NULL}
    };

    if (unlikely(Ns_ParseObjv(lopts, args, interp, 2, objc, objv) != NS_OK)
        || NsConnRequire(interp, NS_CONN_REQUIRE_ALL, &conn) != NS_OK) {
        result = TCL_ERROR;

    } else if (unlikely( Ns_ConnSockPtr(conn) == NULL )) {
        Ns_Log(Warning,
               "NsWriterQueue: called without valid sockPtr, maybe connection already closed");
        Ns_TclPrintfResult(interp, "0");
        result = TCL_OK;

    } else {
        size_t      nrbytes = 0u;
        int         fd = NS_INVALID_FD;

        result = WriterCheckInputParams(interp, fileNameString,
                                        (size_t)size, offset,
                                        &fd, &nrbytes);

        if (likely(result == TCL_OK)) {
            Ns_ReturnCode status;

            /*
             *  The caller requested that we build required headers
             */

            if (headers != 0) {
                Ns_ConnSetTypeHeader(conn, Ns_GetMimeType(fileNameString));
            }
            status = NsWriterQueue(conn, nrbytes, NULL, NULL, fd, NULL, 0,  NULL, 0, NS_TRUE);
            Tcl_SetObjResult(interp, Tcl_NewBooleanObj(status == NS_OK ? 1 : 0));

            if (fd != NS_INVALID_FD) {
                (void) ns_close(fd);
            } else {
                Ns_Log(Warning, "WriterSubmitFileObjCmd called with invalid fd");
            }

        } else if (fd != NS_INVALID_FD) {
            (void) ns_close(fd);
        }
    }

    return result;
}
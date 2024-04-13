WriterSubmitObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int               result = TCL_OK;
    Ns_Conn          *conn;
    Tcl_Obj          *dataObj;
    Ns_ObjvSpec       args[] = {
        {"data", Ns_ObjvObj,  &dataObj, NULL},
        {NULL, NULL, NULL, NULL}
    };

    if (Ns_ParseObjv(NULL, args, interp, 2, objc, objv) != NS_OK
        || NsConnRequire(interp, NS_CONN_REQUIRE_ALL, &conn) != NS_OK) {
        result = TCL_ERROR;

    } else {
        int            size;
        unsigned char *data = Tcl_GetByteArrayFromObj(dataObj, &size);

        if (data != NULL) {
            struct iovec  vbuf;
            Ns_ReturnCode status;

            vbuf.iov_base = (void *)data;
            vbuf.iov_len = (size_t)size;

            status = NsWriterQueue(conn, (size_t)size, NULL, NULL, NS_INVALID_FD,
                                   &vbuf, 1,  NULL, 0, NS_TRUE);
            Tcl_SetObjResult(interp, Tcl_NewBooleanObj(status == NS_OK ? 1 : 0));
        }
    }
    return result;
}
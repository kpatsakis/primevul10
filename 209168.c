AsyncLogfileCloseObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int               fd, result = TCL_OK;
    Ns_ObjvValueRange range = {0, INT_MAX};
    Ns_ObjvSpec args[] = {
        {"fd", Ns_ObjvInt, &fd, &range},
        {NULL, NULL, NULL, NULL}
    };

    if (unlikely(Ns_ParseObjv(NULL, args, interp, 2, objc, objv) != NS_OK)) {
        result = TCL_ERROR;

    } else {
        int rc = ns_close(fd);

        if (rc != 0) {
            Ns_TclPrintfResult(interp, "could not close fd %d: %s",
                               fd, Tcl_PosixError(interp));
            result = TCL_ERROR;
        }
    }
    return result;
}
AsyncLogfileOpenObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int          result = TCL_OK;
    unsigned int flags = O_APPEND;
    char        *fileNameString;
    Tcl_Obj     *flagsObj = NULL;
    Ns_ObjvTable flagTable[] = {
        {"APPEND", O_APPEND},
        {"EXCL",   O_EXCL},
#ifdef O_DSYNC
        {"DSYNC",  O_DSYNC},
#endif
#ifdef O_SYNC
        {"SYNC",   O_SYNC},
#endif
        {"TRUNC",  O_TRUNC},
        {NULL,     0u}
    };
    Ns_ObjvSpec args[] = {
        {"filename", Ns_ObjvString, &fileNameString, NULL},
        {"?flags", Ns_ObjvObj, &flagsObj, NULL},
        //{"mode", Ns_ObjvString, &mode, NULL},
        {NULL, NULL, NULL, NULL}
    };

    if (unlikely(Ns_ParseObjv(NULL, args, interp, 2, objc, objv) != NS_OK)) {
        result = TCL_ERROR;

    } else if (flagsObj != NULL) {
        Tcl_Obj **ov;
        int       oc;

        result = Tcl_ListObjGetElements(interp, flagsObj, &oc, &ov);
        if (result == TCL_OK && oc > 0) {
            int i, opt;

            flags = 0u;
            for (i = 0; i < oc; i++) {
                result = Tcl_GetIndexFromObjStruct(interp, ov[i], flagTable,
                                                   (int)sizeof(flagTable[0]),
                                                   "flag", 0, &opt);
                if (result != TCL_OK) {
                    break;
                } else {
                    flags = flagTable[opt].value;
                }
            }
        }
    }

    if (result == TCL_OK) {
        int fd;

        fd = ns_open(fileNameString, (int)(O_CREAT | O_WRONLY | O_CLOEXEC | flags), 0644);

        if (unlikely(fd == NS_INVALID_FD)) {
            Ns_TclPrintfResult(interp, "could not open file '%s': %s",
                               fileNameString, Tcl_PosixError(interp));
            result = TCL_ERROR;
        } else {
            Tcl_SetObjResult(interp, Tcl_NewIntObj(fd));
        }
    }
    return result;
}
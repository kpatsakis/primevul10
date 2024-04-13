NsTclAsyncLogfileObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    const Ns_SubCmdSpec subcmds[] = {
        {"open",  AsyncLogfileOpenObjCmd},
        {"write", AsyncLogfileWriteObjCmd},
        {"close", AsyncLogfileCloseObjCmd},
        {NULL, NULL}
    };

    return Ns_SubcmdObjv(subcmds, clientData, interp, objc, objv);
}
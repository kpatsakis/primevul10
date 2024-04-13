NsTclDriverObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    const Ns_SubCmdSpec subcmds[] = {
        {"info",       DriverInfoObjCmd},
        {"names",      DriverNamesObjCmd},
        {"threads",    DriverThreadsObjCmd},
        {"stats",      DriverStatsObjCmd},
        {NULL, NULL}
    };

    return Ns_SubcmdObjv(subcmds, clientData, interp, objc, objv);
}
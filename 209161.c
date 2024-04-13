NsTclWriterObjCmd(ClientData clientData, Tcl_Interp *interp,
                  int objc, Tcl_Obj *const* objv)
{
    const Ns_SubCmdSpec subcmds[] = {
        {"list",       WriterListObjCmd},
        {"size",       WriterSizeObjCmd},
        {"streaming",  WriterStreamingObjCmd},
        {"submit",     WriterSubmitObjCmd},
        {"submitfile", WriterSubmitFileObjCmd},
        {"submitfiles",WriterSubmitFilesObjCmd},
        {NULL, NULL}
    };

    return Ns_SubcmdObjv(subcmds, clientData, interp, objc, objv);
}
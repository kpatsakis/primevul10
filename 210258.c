static Jsi_RC jsi_wsFileRead(Jsi_Interp *interp, Jsi_Value *name, Jsi_DString *dStr, jsi_wsCmdObj *cmdPtr, jsi_wsPss *pss) {
    Jsi_StatBuf sb;
    Jsi_RC rc = JSI_ERROR;
    int n = Jsi_Stat(interp, name, &sb);
    if (!n && sb.st_size>0) {
        char fdir[PATH_MAX];
        const char* cr = cmdPtr->curRoot, *fpath=NULL;
        if (!Jsi_FSNative(interp, name) || ((fpath= Jsi_Realpath(interp, name, fdir))
            && cr && !Jsi_Strncmp(fpath, cr, Jsi_Strlen(cr)))) {
            rc = Jsi_FileRead(interp, name, dStr);
            if (rc == JSI_OK && cmdPtr->onModify && Jsi_FSNative(interp, name))
                jsi_wsFileAdd(interp, cmdPtr, name);
        } else
            fprintf(stderr, "Skip read file %s in %s\n", Jsi_ValueString(interp, name, NULL), (cr?cr:""));
    }
    if (cmdPtr->noWarn)
        return JSI_OK;
    return rc;
}
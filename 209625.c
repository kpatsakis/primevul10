static bool jsi_wsIsSSIExt(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, jsi_wsPss *pss, const char *ext) {
    if (cmdPtr->ssiExts) {
        Jsi_Value *mVal = Jsi_ValueObjLookup(interp, cmdPtr->ssiExts, ext, 1);
        if (mVal) {
            bool b = 0;
            if (Jsi_ValueGetBoolean(interp, mVal, &b) != JSI_OK) {
                Jsi_LogWarn("expected bool for ssiExts '%s': disabling all\n", ext);
                Jsi_DecrRefCount(interp, cmdPtr->ssiExts);
                cmdPtr->ssiExts = NULL;
            }
            return b;
        }
    }
    if (ext[0]=='s'  && (!Jsi_Strcmp(ext, "shtml")
        || !Jsi_Strcmp(ext, "scss") || !Jsi_Strcmp(ext, "sjs")))
        return 1;

    return 0;
}
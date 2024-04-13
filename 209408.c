Jsi_RC jsi_JsPreprocessLineCB(Jsi_Interp* interp, char *buf, size_t bsiz, uint ilen, int jOpts[4], int lineNo) {
    const char *jpp = interp->jsppChars;
    if (!jpp[0] || !jpp[1])
        return JSI_OK;
    if (buf[0] && jpp[0] == buf[0] && ilen>2 && buf[ilen-2]==jpp[1]) {
        Jsi_DString dStr = {};
        buf[ilen-2] = 0; // Remove last char and newline.
        Jsi_Value *inStr = Jsi_ValueNewStringDup(interp, buf+1);
        Jsi_IncrRefCount(interp, inStr);
        Jsi_RC rc = Jsi_FunctionInvokeString(interp, interp->jsppCallback, inStr, &dStr);
        if (Jsi_InterpGone(interp))
            return JSI_ERROR;
        if (rc != JSI_OK) {
            Jsi_DSFree(&dStr);
            Jsi_DecrRefCount(interp, inStr);
            return JSI_ERROR;
        }
        Jsi_DecrRefCount(interp, inStr);
        Jsi_DSAppendLen(&dStr, "\n", 1);
        Jsi_Strncpy(buf, Jsi_DSValue(&dStr), bsiz);
        buf[bsiz-1] = 0;
        Jsi_DSFree(&dStr);
    }
    return JSI_OK;
}
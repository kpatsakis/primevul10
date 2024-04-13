static Jsi_RC jsi_wsGetCmd(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, jsi_wsPss* pss, struct lws *wsi,
    const char *inPtr, Jsi_Value *cmd, Jsi_DString *tStr)
{
    Jsi_RC jrc;
    Jsi_Value *retStr = Jsi_ValueNew1(interp);
    // 4 args: ws, id, url, query
    Jsi_Value *vpargs, *vargs[10];
    int n = 0;
    if (cmdPtr->deleted) return JSI_ERROR;
    vargs[n++] = Jsi_ValueNewObj(interp, cmdPtr->fobj);
    vargs[n++] = Jsi_ValueNewNumber(interp, (Jsi_Number)(pss?pss->wid:0));
    vargs[n++]  = Jsi_ValueNewStringDup(interp, inPtr);
    vargs[n++]  = (pss->query?pss->query:Jsi_ValueNewArray(interp, NULL, 0));
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, vargs, n, 0));
    Jsi_IncrRefCount(interp, vpargs);
    jrc = Jsi_FunctionInvoke(interp, cmd, vpargs, &retStr, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    if (Jsi_InterpGone(interp))
        return JSI_ERROR;
    Jsi_DString dStr = {};
    const char *rstr = "";
    if (jrc != JSI_OK)
        rstr = "Error";
    else if (Jsi_ValueIsFalse(interp, retStr)) {
        rstr = "Access denied";
        jrc = JSI_ERROR;
    } else
        rstr = Jsi_ValueString(interp, retStr, NULL);
    if (rstr && tStr && Jsi_Strncmp(rstr, "!!!", 3)==0) {
        Jsi_DSAppend(tStr, rstr+3, NULL);
        jrc = JSI_CONTINUE;
    } else if (rstr && tStr && Jsi_Strncmp(rstr, ">>>", 3)==0) {
        Jsi_DSAppend(tStr, rstr+3, NULL);
        jrc = JSI_SIGNAL;
    } else if (rstr && rstr[0] != 0)
        jsi_wsServeString(pss, wsi, rstr, jrc==JSI_OK?0:404, NULL, NULL);
    else
        jrc = JSI_BREAK;
    Jsi_DecrRefCount(interp, retStr);
    Jsi_DSFree(&dStr);
    return jrc;
}
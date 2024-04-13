static Jsi_RC jsi_wsrecv_callback(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, jsi_wsPss *pss,
    const char *inPtr, int nlen, bool isClose)
{
    Jsi_Value *vpargs, *vargs[10];
    Jsi_Value* func = NULL;
    if (Jsi_InterpGone(interp) || (cmdPtr->deleted && !isClose)) return JSI_ERROR;
    int n = 0;
    if (isClose)
        func = ((pss && pss->onClose)?pss->onClose:cmdPtr->onClose);
    else
        func = ((pss && pss->onRecv)?pss->onRecv:cmdPtr->onRecv);
    if (!func)
        return JSI_OK;
    vargs[n++] = (cmdPtr->deleted?Jsi_ValueNewNull(interp):Jsi_ValueNewObj(interp, cmdPtr->fobj));
    vargs[n++] = Jsi_ValueNewNumber(interp, (Jsi_Number)(pss?pss->wid:0));
    if (!isClose) {
        if (nlen<=0)
            return JSI_OK;
        vargs[n++]  = Jsi_ValueNewBlob(interp, (uchar*)inPtr, nlen);
        if ((cmdPtr->echo||(pss && pss->echo)) && inPtr)
            Jsi_LogInfo("WS-RECV: %s\n", inPtr);
    }
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, vargs, n, 0));
    Jsi_IncrRefCount(interp, vpargs);

    Jsi_Value *ret = Jsi_ValueNew1(interp);
    Jsi_RC rc = Jsi_FunctionInvoke(interp, func, vpargs, &ret, NULL);
    if (rc == JSI_OK && Jsi_ValueIsUndef(interp, ret)==0 && !isClose) {
        /* TODO: should we handle callback return data??? */
    }
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
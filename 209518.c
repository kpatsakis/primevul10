static Jsi_RC InterpCallCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    InterpObj *udf = (InterpObj *)Jsi_UserObjGetData(interp, _this, funcPtr);
    int isthrd;
    Jsi_Interp *sinterp;
    if (udf)
        sinterp = udf->subinterp;
    else
        return Jsi_LogError("Apply Interp.call in a non-subinterp");
    if (Jsi_InterpGone(sinterp))
        return Jsi_LogError("Sub-interp gone");
    isthrd = (interp->threadId != sinterp->threadId);

    Jsi_Value *func = NULL;
    char *fname = NULL;
    func = Jsi_ValueArrayIndex(interp, args, 0);
    fname = Jsi_ValueString(interp, func, NULL);
    if (!fname)
        return Jsi_LogError("function name must be a string");
    if (Jsi_MutexLock(interp, sinterp->Mutex) != JSI_OK)
        return JSI_ERROR;
    Jsi_Value *namLU = Jsi_NameLookup(sinterp, fname);
    Jsi_MutexUnlock(interp, sinterp->Mutex);
    if (namLU == NULL)
        return Jsi_LogError("unknown function: %s", fname);

    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 1);
    char *cp = Jsi_ValueString(interp, arg, NULL);

    if (cp == NULL && !Jsi_ValueIsArray(interp, arg))
        return Jsi_LogError("expected string or array");

    Jsi_Value *vwait = Jsi_ValueArrayIndex(interp, args, 2);
    Jsi_RC rc = JSI_OK;
    bool wait = 0;
    if (vwait && Jsi_GetBoolFromValue(interp, vwait, &wait))
        return JSI_ERROR;

    if (wait) {
        Jsi_DString dStr = {};
        if (cp == NULL)
            cp = (char*)Jsi_ValueGetDString(interp, arg, &dStr, JSI_OUTPUT_JSON);
        if (interp->subOpts.mutexUnlock) Jsi_MutexUnlock(interp, interp->Mutex);
        if (Jsi_MutexLock(interp, sinterp->Mutex) != JSI_OK) {
            if (interp->subOpts.mutexUnlock) Jsi_MutexLock(interp, interp->Mutex);
            return JSI_ERROR;
        }
        Jsi_Value *srPtr = Jsi_ValueNew1(sinterp);
        /* TODO: call from this interp may not be safe if threaded.
         * Could instead use async code below then wait for unlock on se->mutex. */
        rc = Jsi_CommandInvokeJSON(sinterp, fname, cp, &srPtr);
        Jsi_DSSetLength(&dStr, 0);
        Jsi_CleanValue(sinterp, interp, srPtr, ret);
        //Jsi_ValueCopy(interp, *ret, srPtr);
        Jsi_DecrRefCount(sinterp, srPtr);
        Jsi_DSFree(&dStr);
        Jsi_MutexUnlock(interp, sinterp->Mutex);
        if (interp->subOpts.mutexUnlock && Jsi_MutexLock(interp, interp->Mutex) != JSI_OK) {
            Jsi_LogBug("mutex re-get failed");
            return JSI_ERROR;
        }
        return rc;
    }

    /* Post to thread event in sub-interps queue. */
    if (Jsi_MutexLock(interp, sinterp->QMutex) != JSI_OK)
        return JSI_ERROR;

    /* Is an async call. */
    InterpStrEvent *se, *s = (InterpStrEvent *)Jsi_Calloc(1, sizeof(*s));
    // TODO: is s->data inited?
    Jsi_DSInit(&s->data);
    if (!cp) {
        Jsi_ValueGetDString(interp, arg, &s->data, JSI_OUTPUT_JSON);
    } else {
        Jsi_DSSetLength(&s->data, 0);
        Jsi_DSAppend(&s->data, cp, NULL);
    }
    Jsi_DSInit(&s->func);
    Jsi_DSAppend(&s->func, fname, NULL);
    se = sinterp->interpStrEvents;
    if (!se)
        sinterp->interpStrEvents = s;
    else {
        while (se->next)
            se = se->next;
        se->next = s;
    }

    Jsi_MutexUnlock(interp, sinterp->QMutex);
    if (!isthrd) {
        if (SubInterpEvalCallback(sinterp, NULL) != JSI_OK)
            sinterp->threadErrCnt++;
    }
    return JSI_OK;
}
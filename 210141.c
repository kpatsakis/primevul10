Jsi_RC jsi_AliasInvoke(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    AliasCmd *ac = (AliasCmd *)funcPtr->cmdSpec->reserved[2];
    Jsi_Interp *dinterp = ac->dinterp;
    Jsi_Value *nargs = NULL;
    int inc=0, argc = Jsi_ValueGetLength(interp, args);
    if (!ac) {
        Jsi_LogBug("BAD ALIAS INVOKE OF DELETED");
        return JSI_ERROR;
    }
    SIGASSERT(ac,ALIASCMD);
    bool isthrd = (interp != dinterp && interp->threadId != dinterp->threadId);
    Jsi_Value *nrPtr = Jsi_ValueNew1(dinterp);

     if (argc == 0 && ac->args)
        nargs = ac->args;
     else if (argc) {
        if (dinterp == interp)
            Jsi_ValueCopy(interp, nrPtr, args);
        else if (Jsi_CleanValue(interp, dinterp, args, &nrPtr) != JSI_OK)
            return JSI_ERROR;
        if (ac->args && Jsi_ValueGetLength(dinterp, ac->args)) {
            nargs = Jsi_ValueArrayConcat(dinterp, ac->args, nrPtr);
            Jsi_IncrRefCount(dinterp, nargs);
            inc=1;
        } else {
            nargs = nrPtr;
        }
    }

    if (isthrd) {
        /* Post to thread event in sub-interps queue. */
        if (Jsi_MutexLock(interp, dinterp->QMutex) != JSI_OK)
            return JSI_ERROR;

       /* Is an async call. */
        InterpStrEvent *se, *s = (InterpStrEvent *)Jsi_Calloc(1, sizeof(*s));
        // TODO: is s->data inited?
        Jsi_DSInit(&s->data);
        Jsi_ValueGetDString(interp, nargs, &s->data, JSI_OUTPUT_JSON);
        if (inc)
            Jsi_DecrRefCount(dinterp, nargs);
        Jsi_DecrRefCount(dinterp, nrPtr);
        s->acfunc = ac->func;
        Jsi_IncrRefCount(dinterp, ac->func);
        se = dinterp->interpStrEvents;
        if (!se)
            dinterp->interpStrEvents = s;
        else {
            while (se->next)
                se = se->next;
            se->next = s;
        }

        Jsi_MutexUnlock(interp, dinterp->QMutex);
        return JSI_OK;
    }

    if (dinterp != interp) {
        if (interp->subOpts.mutexUnlock) Jsi_MutexUnlock(interp, interp->Mutex);
        if (Jsi_MutexLock(interp, dinterp->Mutex) != JSI_OK) {
            if (interp->subOpts.mutexUnlock) Jsi_MutexLock(interp, interp->Mutex);
            return JSI_ERROR;
        }
    }
    ac->refCount++;
    Jsi_Value *srPtr, **srpPtr = ret;
    if (dinterp != interp) {
        srPtr = Jsi_ValueNew1(dinterp);
        srpPtr = &srPtr;
    }
    Jsi_RC rc = Jsi_FunctionInvoke(dinterp, ac->func, nargs, srpPtr, NULL);
    ac->refCount--;
    if (inc)
        Jsi_DecrRefCount(dinterp, nargs);
    Jsi_DecrRefCount(dinterp, nrPtr);
    if (dinterp != interp) {
        Jsi_MutexUnlock(interp, dinterp->Mutex);
        if (interp->subOpts.mutexUnlock && Jsi_MutexLock(interp, interp->Mutex) != JSI_OK) {
            return JSI_ERROR;
        }
    }
    if (dinterp != interp) {
        Jsi_CleanValue(dinterp, interp, *srpPtr, ret);
        Jsi_DecrRefCount(dinterp, srPtr);
        if (rc != JSI_OK && dinterp->errMsgBuf[0] && interp != dinterp) {
            Jsi_Strcpy(interp->errMsgBuf, dinterp->errMsgBuf);
            interp->errLine = dinterp->errLine;
            interp->errFile = dinterp->errFile;
            dinterp->errMsgBuf[0] = 0;
        }
    }
    return rc;
}
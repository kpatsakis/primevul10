static Jsi_RC InterpEvalCmd_(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr, int flags)
{
    int isFile = flags&2;
    int isUplevel = flags&1;
    int lev = 0;
    bool async = 0;
    Jsi_RC rc = JSI_OK;
    int isthrd;
    Jsi_Interp *sinterp = interp;
    Jsi_ValueMakeUndef(interp, ret);
    InterpObj *udf = (InterpObj *)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (udf)
        sinterp = udf->subinterp;
    if (Jsi_InterpGone(interp) || Jsi_InterpGone(sinterp))
        return Jsi_LogError("Sub-interp gone");
    isthrd = (interp->threadId != sinterp->threadId);
    jsi_Frame *f = sinterp->framePtr;
    Jsi_Value *nw = Jsi_ValueArrayIndex(interp, args, 1);
    if (!isUplevel) {
        if (nw && Jsi_GetBoolFromValue(interp, nw, &async))
            return JSI_ERROR;
    } else {
        if (isthrd)
            return Jsi_LogError("can not use uplevel() with threaded interp");
        Jsi_Number nlev = sinterp->framePtr->level;
        if (nw && Jsi_GetNumberFromValue(interp, nw, &nlev)!=JSI_OK)
            return Jsi_LogError("expected number");
        lev = (int)nlev;
        if (lev <= 0)
            lev = f->level+lev;
        if (lev <= 0 || lev > f->level)
            return Jsi_LogError("level %d not between 1 and %d", (int)nlev, f->level);
    }

    char *cp = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    if (cp==NULL || *cp == 0)
        return JSI_OK;
    if (async && isthrd) {
        /* Post to thread event in sub-interps queue. TODO: could just use event like below... */
        if (Jsi_MutexLock(interp, sinterp->QMutex) != JSI_OK)
            return JSI_ERROR;
        Jsi_DSAppend(&sinterp->interpEvalQ, Jsi_Strlen(Jsi_DSValue(&sinterp->interpEvalQ))?";":"", cp, NULL);
        Jsi_MutexUnlock(interp, sinterp->QMutex);
        return JSI_OK;
    }
    if (interp->subOpts.mutexUnlock) Jsi_MutexUnlock(interp, interp->Mutex);
    if (!isthrd) {
        int ostrict = sinterp->strict;
        sinterp->strict = 0;
        sinterp->level++;
        if (interp->framePtr->tryDepth)
            sinterp->framePtr->tryDepth++;
        if (isFile) {
            int sflags = 0;
            if (!sinterp->includeCnt) {
                sflags = JSI_EVAL_ARGV0|JSI_EVAL_AUTOINDEX;
                sinterp->isMain = 1;
            }
            if (sinterp->debugOpts.debugCallback && !sinterp->includeCnt)  // TODO: safe debugging can't use "source"
                // TODO: we do this in debugger, even though it is illegal for interps to share objects.
                sinterp->autoFiles = Jsi_ValueDup(sinterp, interp->autoFiles);
            sinterp->includeCnt++;
            rc = Jsi_EvalFile(sinterp, Jsi_ValueArrayIndex(interp, args, 0), sflags);
        } else if (isUplevel == 0 || lev <= 1)
            rc = (Jsi_EvalString(sinterp, cp, 0) == 0 ? JSI_OK : JSI_ERROR);
        else {
            rc = (jsi_evalStrFile(sinterp, NULL, cp, 0, lev) == 0 ? JSI_OK : JSI_ERROR);
        }
        sinterp->strict = ostrict;
        if (interp->framePtr->tryDepth) {
            sinterp->framePtr->tryDepth--;
            if (rc != JSI_OK && interp != sinterp) {
                Jsi_Strcpy(interp->errMsgBuf, sinterp->errMsgBuf);
                interp->errLine = sinterp->errLine;
                interp->errFile = sinterp->errFile;
                sinterp->errMsgBuf[0] = 0;
            }
        }
        sinterp->level--;
    } else {
        if (Jsi_MutexLock(interp, sinterp->QMutex) != JSI_OK)
            return JSI_ERROR;
        InterpStrEvent *se, *s = (InterpStrEvent *)Jsi_Calloc(1, sizeof(*s));
        SIGINIT(s,INTERPSTREVENT);
        s->isExec = 1;
        s->tryDepth = interp->framePtr->tryDepth;
        Jsi_DSInit(&s->data);
        Jsi_DSAppend(&s->data, cp, NULL);
        Jsi_DSInit(&s->func);
        //s->mutex = Jsi_MutexNew(interp, -1, JSI_MUTEX_RECURSIVE);
        //Jsi_MutexLock(s->mutex);
        se = sinterp->interpStrEvents;
        if (!se)
            sinterp->interpStrEvents = s;
        else {
            while (se->next)
                se = se->next;
            se->next = s;
        }

        Jsi_MutexUnlock(interp, sinterp->QMutex);
        while (s->isExec)      /* Wait until done. TODO: timeout??? */
            Jsi_Sleep(interp, 1);
        rc = (s->rc == 0 ? JSI_OK : JSI_ERROR);
        if (rc != JSI_OK)
            Jsi_LogError("eval failed: %s", Jsi_DSValue(&s->data));
        Jsi_DSFree(&s->func);
        Jsi_DSFree(&s->data);
        Jsi_Free(s);
    }

    if (interp->subOpts.mutexUnlock && Jsi_MutexLock(interp, interp->Mutex) != JSI_OK) {
        return JSI_ERROR;
    }

    if (Jsi_InterpGone(sinterp))
    {
        /* TODO: perhaps exit() be able to delete. */
        //Jsi_InterpDelete(sinterp);
        return JSI_OK;
    }
    /*if (rc != JSI_OK && !async)
        return rc;*/
    if (sinterp->retValue->vt != JSI_VT_UNDEF) {
        if (sinterp == interp)
            Jsi_ValueCopy(interp, *ret, sinterp->retValue);
        else
            Jsi_CleanValue(sinterp, interp, sinterp->retValue, ret);
    }
    return rc;
}
static Jsi_RC SubInterpEvalCallback(Jsi_Interp *interp, void* data)
{
    Jsi_RC rc = JSI_OK;
    Jsi_DString dStr = {};
    if (Jsi_MutexLock(interp, interp->QMutex) != JSI_OK)
        return JSI_ERROR;
    char *cp = Jsi_DSValue(&interp->interpEvalQ);
    if (*cp) {
        Jsi_DSAppend(&dStr, cp, NULL);
        Jsi_DSSetLength(&interp->interpEvalQ, 0);
        interp->threadEvalCnt++;
        Jsi_MutexUnlock(interp, interp->QMutex);
        if (Jsi_EvalString(interp, Jsi_DSValue(&dStr), 0) != JSI_OK)
            rc = JSI_ERROR;
        Jsi_DSSetLength(&dStr, 0);
        if (Jsi_MutexLock(interp, interp->QMutex) != JSI_OK)
            return JSI_ERROR;
    }
    Jsi_MutexUnlock(interp, interp->QMutex);
    Jsi_DSFree(&dStr);
    if (Jsi_MutexLock(interp, interp->QMutex) != JSI_OK)
        return JSI_ERROR;

    /* Process subevents. */
    InterpStrEvent *oldse, *se = interp->interpStrEvents;
    Jsi_MutexUnlock(interp, interp->QMutex);
    while (se) {
        oldse = se;
        int isExec = se->isExec;
        if (se->acdata) {
            jsi_AliasCreateCmd(interp, Jsi_DSValue(&se->func), se->acdata);
        }
        else if (se->acfunc) {
            if (JSI_OK != Jsi_FunctionInvokeJSON(interp, se->acfunc, Jsi_DSValue(&se->data), NULL))
                rc = JSI_ERROR;
            Jsi_DecrRefCount(interp, se->acfunc);
        }
        else if (isExec) {
            if (se->tryDepth)
                interp->framePtr->tryDepth++;
            se->rc = Jsi_EvalString(interp, Jsi_DSValue(&se->data), 0);
            Jsi_DSSetLength(&se->data, 0);
            if (se->rc != JSI_OK && se->tryDepth) {
                Jsi_DSAppend(&se->data, interp->errMsgBuf, NULL);
                se->errLine = interp->errLine;
                se->errFile = interp->errFile;
            } else {
                Jsi_ValueGetDString(interp, interp->retValue, &se->data, JSI_OUTPUT_JSON);
            }
            if (se->tryDepth)
                interp->framePtr->tryDepth--;

        /* Otherwise, async calls. */
        /*} else if (se->objData) {
            if (JSI_OK != Jsi_CommandInvoke(interp, Jsi_DSValue(&se->func), se->objData, NULL))
                rc = JSI_ERROR;*/
        } else {
            const char *scp = Jsi_DSValue(&se->data);
            if (JSI_OK != Jsi_CommandInvokeJSON(interp, Jsi_DSValue(&se->func), scp, NULL))
                rc = JSI_ERROR;
        }
        if (!isExec) {
            Jsi_DSFree(&se->func);
            Jsi_DSFree(&se->data);
        }
        if (Jsi_MutexLock(interp, interp->QMutex) != JSI_OK)
            return JSI_ERROR;
        interp->interpStrEvents = se->next;
        if (!isExec) Jsi_Free(se);
        se = interp->interpStrEvents;
        Jsi_MutexUnlock(interp, interp->QMutex);
        if (isExec)
            oldse->isExec = 0;
    }

    return rc;
}
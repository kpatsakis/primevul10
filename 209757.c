Jsi_RC Jsi_EvalCmdJSON(Jsi_Interp *interp, const char *cmd, const char *jsonArgs, Jsi_DString *dStr, int flags)
{
    if (Jsi_MutexLock(interp, interp->Mutex) != JSI_OK)
        return JSI_ERROR;
    Jsi_Value *nrPtr = Jsi_ValueNew1(interp);
    Jsi_RC rc = Jsi_CommandInvokeJSON(interp, cmd, jsonArgs, &nrPtr);
    Jsi_DSInit(dStr);
    Jsi_ValueGetDString(interp, nrPtr, dStr, flags /*JSI_OUTPUT_JSON*/);
    Jsi_DecrRefCount(interp, nrPtr);
    Jsi_MutexUnlock(interp, interp->Mutex);
    return rc;
}
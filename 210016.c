Jsi_RC Jsi_FunctionInvokeString(Jsi_Interp *interp, Jsi_Value *func, Jsi_Value *arg, Jsi_DString *dStr)
{
    if (interp->deleting)
        return JSI_ERROR;
    Jsi_Value *vpargs, *frPtr = Jsi_ValueNew1(interp);
    Jsi_RC rc;
    if (!arg) {
        if (!interp->nullFuncArg) {
            interp->nullFuncArg = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, NULL, 0, 0));
            Jsi_IncrRefCount(interp, interp->nullFuncArg);
        }
        vpargs = interp->nullFuncArg;
    } else {
        vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, &arg, 1, 1));
    }
    Jsi_IncrRefCount(interp, vpargs);
    rc = Jsi_FunctionInvoke(interp, func, vpargs, &frPtr, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    if (rc != JSI_OK)
        Jsi_LogError("function call failed");
    else
        Jsi_ValueGetDString(interp, frPtr, dStr, 0);
    Jsi_DecrRefCount(interp, frPtr);
    return rc;
}
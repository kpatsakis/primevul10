int Jsi_FunctionInvokeBool(Jsi_Interp *interp, Jsi_Value *func, Jsi_Value *arg)
{
    if (interp->deleting)
        return JSI_ERROR;
    Jsi_Value *vpargs, *frPtr = Jsi_ValueNew1(interp);
    Jsi_RC rc;
    int bres = 0;
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
    if (rc == JSI_OK)
        bres = Jsi_ValueIsTrue(interp, frPtr);
    else {
        bres = 2;
        Jsi_LogError("function call failed");
    }
    Jsi_DecrRefCount(interp, frPtr);
    if (Jsi_InterpGone(interp))
        return JSI_ERROR;
    return bres;
}
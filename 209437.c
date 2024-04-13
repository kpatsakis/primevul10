static Jsi_RC InterpValueCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    InterpObj *udf = (InterpObj *)Jsi_UserObjGetData(interp, _this, funcPtr);
    Jsi_Interp *sinterp = interp;
    if (udf) {
        if (!udf->subinterp)
        return Jsi_LogError("Sub-interp gone");
        sinterp = udf->subinterp;
        if (interp->threadId != udf->subinterp->threadId)
            return Jsi_LogError("value not supported with threads");
    }
    Jsi_Value *nw = Jsi_ValueArrayIndex(interp, args, 1);
    jsi_Frame *f = sinterp->framePtr;
    Jsi_Number nlev = sinterp->framePtr->level;
    if (nw && Jsi_GetNumberFromValue(interp, nw, &nlev))
        return JSI_ERROR;
    int lev = (int)nlev;
    if (lev <= 0)
        lev = f->level+lev;
    if (lev <= 0 || lev > f->level)
        return Jsi_LogError("level %d not between 1 and %d", (int)nlev, f->level);
    while (f->level != lev  && f->parent)
        f = f->parent;

    const char* arg = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_Value *val = NULL;
    if (arg) {
        if (f == sinterp->framePtr)
            val = Jsi_NameLookup(sinterp, arg);
        else {
            jsi_Frame *of = sinterp->framePtr;
            sinterp->framePtr = f;
            val = Jsi_NameLookup(sinterp, arg);
            sinterp->framePtr = of;
        }
    }
    if (!val)
        return Jsi_LogError("unknown var: %s", arg);
    if (sinterp == interp) {
        Jsi_ValueCopy(interp, *ret, val);
        return JSI_OK;
    }
    Jsi_CleanValue(sinterp, interp, val, ret);
    return JSI_OK;
}
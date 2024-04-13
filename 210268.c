Jsi_RC Jsi_ValueInsert(Jsi_Interp *interp, Jsi_Value *target, const char *key, Jsi_Value *val, int flags)
{
    if (target == NULL)
        target = interp->csc;
    if (target->vt != JSI_VT_OBJECT) {
        if (interp->strict)
            Jsi_LogWarn("Target is not object");
        return JSI_ERROR;
    }
    target->f.flag |= flags;
    if (Jsi_ObjInsert(interp, target->d.obj, key, val, flags))
        return JSI_OK;
    return JSI_ERROR;
}
static Jsi_RC jsi_ValueToVerify(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    if (inStr)
        return JSI_ERROR;
    Jsi_Value **s = (Jsi_Value**)((char*)record) + spec->offset;
    Jsi_ValueHandlerProc *vfunc = (Jsi_ValueHandlerProc*)spec->data;
    if (!vfunc) 
        return Jsi_LogError("custom value spec did not set data: %s", spec->name);
    if (!inValue) {
        if (*s)
            Jsi_DecrRefCount(interp, *s);
        *s = NULL;
        return JSI_OK;
    }
    if (vfunc(interp, inValue, spec, record) != JSI_OK)
        return JSI_ERROR;
    *s = inValue;
    if (*s)
        Jsi_IncrRefCount(interp, *s);
    return JSI_OK;
}
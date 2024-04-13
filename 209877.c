Jsi_RC Jsi_GetLongFromValue(Jsi_Interp* interp, Jsi_Value *value, long *n)
{
    /* TODO: inefficient to convert to double then back. */
    if (!value)
        return JSI_ERROR;
    if (!interp->strict)
        jsi_ValueToOInt32(interp, value);
    if (!Jsi_ValueIsNumber(interp, value))
    
        return Jsi_LogError("invalid number");
    *n = (long)(value->vt == JSI_VT_NUMBER ? value->d.num : value->d.obj->d.num);
    return JSI_OK;
}
Jsi_RC Jsi_GetDoubleFromValue(Jsi_Interp* interp, Jsi_Value *value, Jsi_Number *n)
{
    if (!value)
        return JSI_ERROR;
    if (!interp->strict)
        Jsi_ValueToNumber(interp, value);
    if (!Jsi_ValueIsNumber(interp, value))
    
        return Jsi_LogError("invalid number");
    *n = (value->vt == JSI_VT_NUMBER ? value->d.num : value->d.obj->d.num);
    return JSI_OK;
}
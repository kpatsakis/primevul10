Jsi_RC Jsi_GetWideFromValue(Jsi_Interp* interp, Jsi_Value *value, Jsi_Wide *n)
{
    if (!value)
        return JSI_ERROR;
    if (!interp->strict)
        jsi_ValueToOInt32(interp, value);
    if (!Jsi_ValueIsNumber(interp, value))
    
        return Jsi_LogError("invalid number");
    *n = (Jsi_Wide)(value->vt == JSI_VT_NUMBER ? value->d.num : value->d.obj->d.num);
    return JSI_OK;

}
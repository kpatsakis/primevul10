Jsi_RC Jsi_GetNumberFromValue(Jsi_Interp* interp, Jsi_Value *value, Jsi_Number *n)
{
    if (!value)
        return JSI_ERROR;

    if (value->vt == JSI_VT_NUMBER) {
        *n = value->d.num;
        return JSI_OK;
    }
    if (value->vt == JSI_VT_OBJECT && value->d.obj->ot == JSI_OT_NUMBER) {
        *n = value->d.obj->d.num;
        return JSI_OK;
    }
    if (interp)
        Jsi_LogError("invalid number");
    return JSI_ERROR;
}
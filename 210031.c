Jsi_RC Jsi_GetBoolFromValue(Jsi_Interp* interp, Jsi_Value *value, bool *n)
{
    if (!value)
        return JSI_ERROR;

    if (value->vt == JSI_VT_BOOL) {
        *n = value->d.val;
        return JSI_OK;
    }
    if (value->vt == JSI_VT_OBJECT && value->d.obj->ot == JSI_OT_BOOL) {
        *n = value->d.obj->d.val;
        return JSI_OK;
    }
    Jsi_LogError("invalid bool");
    return JSI_ERROR;
}
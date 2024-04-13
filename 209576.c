Jsi_RC Jsi_GetStringFromValue(Jsi_Interp* interp, Jsi_Value *value, const char **n)
{
    if (!value)
        return JSI_ERROR;
    if (value->vt == JSI_VT_STRING)
    {
        *n = (const char*)value->d.s.str;
         return JSI_OK;
    }
    if (value->vt == JSI_VT_OBJECT && value->d.obj->ot == JSI_OT_STRING) {
        *n = value->d.obj->d.s.str;
        return JSI_OK;
    }
    Jsi_LogError("invalid string");
    return JSI_ERROR;
}
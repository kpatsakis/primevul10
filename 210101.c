Jsi_RC Jsi_ValueGetBoolean(Jsi_Interp *interp, Jsi_Value *pv, bool *val)
{
    if (!pv) return JSI_ERROR;
    if (pv->vt == JSI_VT_BOOL)
        *val = pv->d.val;
    else if (pv->vt == JSI_VT_OBJECT && pv->d.obj->ot == JSI_OT_BOOL)
        *val = pv->d.obj->d.val;
    else 
        return JSI_ERROR;
    return JSI_OK;
}
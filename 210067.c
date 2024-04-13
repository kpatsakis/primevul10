Jsi_RC Jsi_ValueGetNumber(Jsi_Interp *interp, Jsi_Value *pv, Jsi_Number *val)
{
    if (!pv) return JSI_ERROR;
    if (pv->vt == JSI_VT_NUMBER)
        *val = pv->d.num;
    else if (pv->vt == JSI_VT_OBJECT && pv->d.obj->ot == JSI_OT_NUMBER)
        *val = pv->d.obj->d.num;
    else 
        return JSI_ERROR;
    return JSI_OK;
}
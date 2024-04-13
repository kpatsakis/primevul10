bool Jsi_ValueIsBoolean(Jsi_Interp *interp, Jsi_Value *pv)
{
    return (pv->vt == JSI_VT_BOOL || (pv->vt == JSI_VT_OBJECT && pv->d.obj->ot == JSI_OT_BOOL));
}
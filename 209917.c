bool Jsi_ValueIsString(Jsi_Interp *interp, Jsi_Value *pv)
{
    return (pv->vt == JSI_VT_STRING || (pv->vt == JSI_VT_OBJECT && pv->d.obj->ot == JSI_OT_STRING));
}
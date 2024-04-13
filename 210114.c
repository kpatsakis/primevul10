bool Jsi_ValueIsNumber(Jsi_Interp *interp, Jsi_Value *pv)
{
    return (pv->vt == JSI_VT_NUMBER || (pv->vt == JSI_VT_OBJECT && pv->d.obj->ot == JSI_OT_NUMBER));
}
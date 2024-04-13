bool Jsi_ValueIsNull(Jsi_Interp *interp, Jsi_Value *pv)
{
    return (pv->vt == JSI_VT_NULL);
}
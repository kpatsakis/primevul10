bool Jsi_ValueIsUndef(Jsi_Interp *interp, Jsi_Value *pv)
{
    return (pv->vt == JSI_VT_UNDEF);
}
bool Jsi_ValueIsFalse(Jsi_Interp *interp, Jsi_Value *v)
{
    if (v->vt == JSI_VT_BOOL)  return v->d.val ? 0:1;
    return 0;
}
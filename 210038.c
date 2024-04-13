Jsi_RC Jsi_ValueToNumber(Jsi_Interp *interp, Jsi_Value *v)
{
    if (v->vt == JSI_VT_NUMBER) return JSI_OK;
    Jsi_Number a = Jsi_ValueToNumberInt(interp, v, 0);
    Jsi_ValueReset(interp, &v);
    Jsi_ValueMakeNumber(interp, &v, a);
    return JSI_OK;
}
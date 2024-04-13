Jsi_Value* Jsi_ValueMakeNumber(Jsi_Interp *interp, Jsi_Value **vPtr, Jsi_Number n) {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!v)
        v = Jsi_ValueNew(interp);
    else
        Jsi_ValueReset(interp, vPtr);
    v->vt = JSI_VT_NUMBER;
    v->d.num = n;
    return v;
}
Jsi_Value* Jsi_ValueMakeNull(Jsi_Interp *interp, Jsi_Value **vPtr) {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!v)
        v = Jsi_ValueNew(interp);
    else
        Jsi_ValueReset(interp, vPtr);
    v->vt = JSI_VT_NULL;
    return v;
}
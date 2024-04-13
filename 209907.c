Jsi_Value* Jsi_ValueMakeUndef(Jsi_Interp *interp, Jsi_Value **vPtr) {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!v)
        v = Jsi_ValueNew(interp);
    else {
        if (v->vt == JSI_VT_UNDEF) return v;
        Jsi_ValueReset(interp, vPtr);
    }
    return v;
}
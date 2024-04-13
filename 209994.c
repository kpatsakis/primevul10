Jsi_Value* Jsi_ValueMakeBool(Jsi_Interp *interp, Jsi_Value **vPtr, int b) {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!v)
        v = Jsi_ValueNew(interp);
    else
        Jsi_ValueReset(interp, vPtr);
    v->vt = JSI_VT_BOOL;
    v->d.val = b;
    return v;
}
Jsi_Value* Jsi_ValueNewBoolean(Jsi_Interp *interp, int bval) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    v->vt = JSI_VT_BOOL;
    v->d.val = bval;
    return v;
}
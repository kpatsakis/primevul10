Jsi_Value* Jsi_ValueNewNull(Jsi_Interp *interp) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    v->vt = JSI_VT_NULL;
    return v;
}
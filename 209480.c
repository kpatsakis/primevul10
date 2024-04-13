Jsi_Value* Jsi_ValueNewNumber(Jsi_Interp *interp, Jsi_Number n) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    v->vt = JSI_VT_NUMBER;
    v->d.num = n;
    return v;
}
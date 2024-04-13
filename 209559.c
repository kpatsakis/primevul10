Jsi_Value* Jsi_ValueNewObj(Jsi_Interp *interp, Jsi_Obj *o) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    Jsi_ValueMakeObject(interp, &v, o);
    return v;
}
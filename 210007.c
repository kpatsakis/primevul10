Jsi_Value* Jsi_ValueMakeStringKey(Jsi_Interp *interp, Jsi_Value **vPtr, const char *s) {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!v)
        v = Jsi_ValueNew(interp);
    else
        Jsi_ValueReset(interp, vPtr);
    v->vt = JSI_VT_STRING;
    v->d.s.str = (char*)Jsi_KeyAdd(interp,s);
    v->d.s.len = Jsi_Strlen(s);
    v->f.bits.isstrkey = 1;
    return v;
}
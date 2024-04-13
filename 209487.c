Jsi_Value* Jsi_ValueNewStringKey(Jsi_Interp *interp, const char *s) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    v->vt = JSI_VT_STRING;
    v->d.s.str = (char*)Jsi_KeyAdd(interp,s);
    v->d.s.len = Jsi_Strlen(s);
    v->f.bits.isstrkey = 1;
    return v;
}
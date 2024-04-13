Jsi_Value* Jsi_ValueNewStringConst(Jsi_Interp *interp, const char *s, int len) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    v->vt = JSI_VT_STRING;
    v->d.s.str = (char*)s;
    v->d.s.len = (len<0?Jsi_Strlen(s):(uint)len);
    v->f.bits.isstrkey = 1;
    return v;
}
Jsi_Value* Jsi_ValueNewString(Jsi_Interp *interp, const char *s, int len) {
    assert(s);
    Jsi_Value *v = Jsi_ValueNew(interp);
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_STRING);
    Jsi_ValueMakeObject(interp, &v, obj);
    obj->d.s.str = (char*)s;
    obj->d.s.len = (len<0?Jsi_Strlen(s):(uint)len);
    return v;
}
Jsi_Value* Jsi_ValueNewBlob(Jsi_Interp *interp, unsigned char *s, uint len) {
    Jsi_Value *v = Jsi_ValueNew(interp);
    Jsi_Obj *o = Jsi_ObjNewType(interp, JSI_OT_STRING);
    Jsi_ValueMakeObject(interp, &v, o);
    o->d.s.str = (char*)Jsi_Malloc(len+1);
    memcpy(o->d.s.str, (char*)s, len);
    o->d.s.str[len] = 0;
    o->d.s.len = len;
    o->isBlob = 1;
    return v;
}
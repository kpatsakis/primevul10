Jsi_Value* Jsi_ValueMakeBlob(Jsi_Interp *interp, Jsi_Value **vPtr, unsigned char *s, int len) {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!v)
        v = Jsi_ValueNew(interp);
    else
        Jsi_ValueReset(interp, vPtr);
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_STRING);
    Jsi_ValueMakeObject(interp, &v, obj);
    obj->d.s.str = (char*)s;
    obj->d.s.len = len;
    obj->isBlob = 1;
    return v;
}
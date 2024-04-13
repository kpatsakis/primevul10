Jsi_Value* Jsi_ValueMakeDStringObject(Jsi_Interp *interp, Jsi_Value **vPtr, Jsi_DString *dsPtr)  {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    Jsi_Obj *obj;
    if (!v)
        v = Jsi_ValueNew(interp);
    else {
        assert(v->vt <= JSI_VT__MAX);
        if (v->vt == JSI_VT_OBJECT && v->d.obj->ot == JSI_OT_STRING
            && v->d.obj->refcnt == 1
        ) {
            Jsi_ObjFromDS(dsPtr, v->d.obj);
            return v;
        }
        Jsi_ValueReset(interp, &v);
    }
    obj = Jsi_ObjNewType(interp, JSI_OT_STRING);
    Jsi_ObjFromDS(dsPtr, obj);
    Jsi_ValueMakeObject(interp, &v, obj);
    return v;
}
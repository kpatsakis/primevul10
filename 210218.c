Jsi_Value* Jsi_ValueMakeArrayObject(Jsi_Interp *interp, Jsi_Value **vPtr, Jsi_Obj *o)  {
    Jsi_Value *v = (vPtr?*vPtr:NULL);
    if (!o)
        o = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    if (!v)
        v = Jsi_ValueNew(interp);
    else {
       if (v->vt == JSI_VT_OBJECT && o == v->d.obj) {
            if (!o->isarrlist) {
                if (o->tree)
                    Jsi_TreeDelete( o->tree);
                o->tree = NULL;
                o->__proto__ = interp->Array_prototype;
                o->isarrlist = 1;
            }
            return v;
        }
        Jsi_ValueReset(interp, vPtr);
    }
    v->vt = JSI_VT_OBJECT;
    v->d.obj = o;
    o->ot = JSI_OT_OBJECT;
    o->__proto__ = interp->Array_prototype;
    o->isarrlist = 1;
    Jsi_ObjArraySizer(interp, o, 0);
    Jsi_ObjIncrRefCount(interp,v->d.obj);
    return v;
}
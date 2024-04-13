void jsi_VALCHK(Jsi_Value *val) {
    SIGASSERTV(val,VALUE);
    assert(val->vt <= JSI_VT__MAX);
    if (val->vt == JSI_VT_OBJECT)
        OBJCHK(val->d.obj);
}
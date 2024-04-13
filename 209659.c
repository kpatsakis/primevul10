void jsi_OBJCHK(Jsi_Obj *obj) {
    SIGASSERTV(obj,OBJ);
    assert(obj->ot <= JSI_OT__MAX);
}
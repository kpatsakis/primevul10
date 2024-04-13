int Jsi_ValueInstanceOf( Jsi_Interp *interp, Jsi_Value* v1, Jsi_Value* v2)
{
    Jsi_Value *proto, *sproto;
    if (v1->vt != JSI_VT_OBJECT || v2->vt != JSI_VT_OBJECT  || v2->d.obj->ot != JSI_OT_FUNCTION)
        return 0;
    proto = Jsi_ValueObjLookup(interp, v2, "prototype", 0);
    if (!proto)
        return 0;
    sproto = v1->d.obj->__proto__ ;
    while (sproto) {
        if (sproto == proto)
            return 1;
        if (sproto->vt != JSI_VT_OBJECT)
            return 0;
        sproto = sproto->d.obj->__proto__;
    }
    return 0;
}
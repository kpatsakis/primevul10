bool Jsi_ValueIsObjType(Jsi_Interp *interp, Jsi_Value *v, Jsi_otype otype)
{
    if (v == NULL || v->vt != JSI_VT_OBJECT)
        return 0;
    if (otype != JSI_OT_ARRAY)
        return (v->d.obj->ot == otype);
    if (v->d.obj->ot != JSI_OT_OBJECT || !v->d.obj->isarrlist)
        return 0;
    return 1;
}
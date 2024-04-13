bool Jsi_ValueIsArray(Jsi_Interp *interp, Jsi_Value *v)  {
    return (v->vt == JSI_VT_OBJECT && v->d.obj->ot == JSI_OT_OBJECT && v->d.obj->isarrlist);
}
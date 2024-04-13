Jsi_Obj *Jsi_ValueGetObj(Jsi_Interp *interp, Jsi_Value* v)
{
    if (v->vt == JSI_VT_OBJECT) {
        return v->d.obj;
    }
    return NULL;
}
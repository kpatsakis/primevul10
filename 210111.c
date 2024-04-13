Jsi_Tree *Jsi_TreeFromValue(Jsi_Interp *interp, Jsi_Value *v)
{
    if (!Jsi_ValueIsObjType(interp, v, JSI_OT_OBJECT))
        return NULL;
    return v->d.obj->tree;
}
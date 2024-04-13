Jsi_Value *Jsi_NameLookup2(Jsi_Interp *interp, const char *name, const char *inObj)
{
    Jsi_Value *v;
    if (!name)
        return NULL;
    if (!inObj)
        return Jsi_VarLookup(interp, name);
    v = Jsi_VarLookup(interp, inObj);
    if (!v)
        return NULL;
    if (Jsi_ValueIsArray(interp, v)) {
        int n;
        if (!isdigit(name[0]))
            return NULL;
        if (Jsi_GetInt(interp, name, &n, 0) != JSI_OK)
            return NULL;
        if (n>=0 && n<(int)v->d.obj->arrCnt)
            return v->d.obj->arr[n];
        return NULL;
    }
    if (v->vt != JSI_VT_OBJECT || (v->d.obj->ot != JSI_OT_OBJECT && v->d.obj->ot != JSI_OT_FUNCTION))
        return NULL;
    return Jsi_ValueObjLookup(interp, v, name, 0);
}
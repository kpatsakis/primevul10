Jsi_Value *Jsi_ValueArrayIndex(Jsi_Interp *interp, Jsi_Value *args, int index)
{
    Jsi_Obj *obj = args->d.obj;
    Jsi_Value *v;
    assert(args->vt == JSI_VT_OBJECT);
    if (obj->isarrlist && obj->arr)
        return ((index < 0 || (uint)index >= obj->arrCnt) ? NULL : obj->arr[index]);
    char unibuf[JSI_MAX_NUMBER_STRING];
    Jsi_NumberItoA10(index, unibuf, sizeof(unibuf));
    v = Jsi_TreeObjGetValue(args->d.obj, unibuf, 0);
    return v;
}
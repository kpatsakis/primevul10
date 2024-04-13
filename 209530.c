Jsi_Value *Jsi_ValueNewArray(Jsi_Interp *interp, const char **items, int count)
{
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    int i = 0;
    if (count<0) {
        count = 0;
        while (items[count])
            count++;
    }
    if (Jsi_ObjArraySizer(interp, obj, count) <= 0) {
        Jsi_ObjFree(interp, obj);
        return NULL;
    }
    for (i = 0; i < count; ++i) {
        obj->arr[i] = Jsi_ValueNewStringDup(interp, items[i]);
        Jsi_IncrRefCount(interp, obj->arr[i]);
    }
    obj->arrCnt = count;
    assert(obj->arrCnt<=obj->arrMaxSize);
    return Jsi_ValueMakeArrayObject(interp, NULL, obj);
}
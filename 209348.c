void Jsi_ShiftArgs(Jsi_Interp *interp, Jsi_Value *v) {
    if (!v)
        v = interp->args;
    if (v==NULL || v->vt != JSI_VT_OBJECT || v->d.obj->arr == NULL || v->d.obj->arrCnt <= 0)
        return;
    Jsi_Obj *obj = v->d.obj;
    int n = v->d.obj->arrCnt;
    n--;
    v = obj->arr[0];
    if (v)
        Jsi_DecrRefCount(interp, v);
    if (n>0)
        memmove(obj->arr, obj->arr+1, n*sizeof(Jsi_Value*));
    obj->arr[n] = NULL;
    Jsi_ObjSetLength(interp, obj, n);
}
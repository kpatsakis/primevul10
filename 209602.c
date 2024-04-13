Jsi_RC Jsi_ValueGetKeys(Jsi_Interp *interp, Jsi_Value *target, Jsi_Value *ret)
{
    uint i, n = 0;
    Jsi_IterObj *io;
    if (target->vt != JSI_VT_OBJECT)
        return JSI_ERROR;
    Jsi_Obj *to = target->d.obj;
    Jsi_Obj *r = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    Jsi_ValueMakeArrayObject(interp, &ret, r);
    if (to->arr) {
        for (i=0; i<to->arrCnt; i++)
            if (to->arr[i]) n++;
        if (Jsi_ObjArraySizer(interp, r, n) <= 0) {
            Jsi_LogError("too long");
            Jsi_ValueMakeUndef(interp, &ret);
            return JSI_ERROR;
        }
        for (i=0, n=0; i<to->arrCnt; i++) {
            if (to->arr[i]) {
                r->arr[n] = Jsi_ValueNewNumber(interp, (Jsi_Number)i);
                Jsi_IncrRefCount(interp, r->arr[n]);
                n++;
            }
        }
        r->arrCnt = n;
        return JSI_OK;
    }
    io = Jsi_IterObjNew(interp, NULL);
    Jsi_IterGetKeys(interp, target, io, 0);
    if (Jsi_ObjArraySizer(interp, r, io->count) <= 0) {
        Jsi_LogError("too long");
        Jsi_ValueMakeUndef(interp, &ret);
        return JSI_ERROR;
    }
    for (i=0; i<io->count; i++) {
        r->arr[i] = (io->keys[i] ? Jsi_ValueNewStringKey(interp, io->keys[i]) : NULL);
        Jsi_IncrRefCount(interp, r->arr[i]);
    }
    io->count = 0;
    r->arrCnt = i;
    Jsi_IterObjFree(io);
    return JSI_OK;
}
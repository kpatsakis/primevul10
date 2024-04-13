void jsi_ValueObjGetKeys(Jsi_Interp *interp, Jsi_Value *target, Jsi_Value *ret, bool isof)
{
    Jsi_IterObj *io = Jsi_IterObjNew(interp, NULL);
    Jsi_Obj *to = target->d.obj;
    
    if (target->vt != JSI_VT_UNDEF && target->vt != JSI_VT_NULL) {

        if (target->vt == JSI_VT_OBJECT && to->arr) {
            io->isArrayList = 1;
            io->count = to->arrCnt;
        } else {
            if (isof &&interp->strict)
                Jsi_LogWarn("non-array in 'for...of'");
            Jsi_IterGetKeys(interp, target, io, 0);
        }
    }
    io->obj = to;
    io->isof = isof;
    Jsi_Obj *r = Jsi_ObjNew(interp);
    r->ot = JSI_OT_ITER;
    r->d.iobj = io;
    Jsi_ValueMakeObject(interp, &ret, r);
}
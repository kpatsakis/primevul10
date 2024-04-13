static void jsi_ValueCopyMove(Jsi_Interp *interp, Jsi_Value *to, Jsi_Value *from, int isCopy )  {
    if (!from) {
        Jsi_ValueMakeUndef(interp, &to);
        return;
    }
    VALCHK(from);
    if( to == from) return;
    int ocnt = to->refCnt;
    Jsi_Value *ovt = NULL;
    assert(ocnt>0);
    int toVt = to->vt;
    if (toVt == JSI_VT_VARIABLE) {
        ovt = to->d.lval;
        Jsi_IncrRefCount(interp, ovt);
    }
    Jsi_ValueMakeUndef(interp, &to);
#ifdef JSI_MEM_DEBUG
    memcpy(to, from, sizeof(*to)-sizeof(to->VD));
    to->VD.label3 = from->VD.func;
#else
    *to = *from;
#endif
    if (isCopy) {
        if (to->refCnt) {
            switch (to->vt) {
                case JSI_VT_STRING:
                    if (!to->f.bits.isstrkey) {
                        to->d.s.str = Jsi_StrdupLen(to->d.s.str, to->d.s.len);
                    }
                    break;
                case JSI_VT_OBJECT:
                    Jsi_ObjIncrRefCount(interp,to->d.obj);
                    break;
                case JSI_VT_VARIABLE:
                    Jsi_IncrRefCount(interp,to->d.lval);
                    break;
                default:
                    break;
            }
        }
        to->refCnt = ocnt;
        if (ovt)
            Jsi_DecrRefCount(interp, ovt);
    } else {
        to->refCnt = ocnt;
        if (ovt)
            Jsi_DecrRefCount(interp, ovt);
        ocnt = from->refCnt;
#ifdef JSI_MEM_DEBUG
        memset(from, 0, sizeof(*to)-sizeof(to->VD));
#else
        memset(from, 0, sizeof(*to));
#endif
        SIGINIT(from, VALUE);
        from->refCnt = ocnt;
    }
}
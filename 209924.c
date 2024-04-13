static void ValueFree(Jsi_Interp *interp, Jsi_Value* v)
{
    SIGASSERTV(v,VALUE);
    //printf("FREE: %d\n", interp->valueCnt);
    switch (v->vt) {
        case JSI_VT_OBJECT:
            Jsi_ObjDecrRefCount(interp, v->d.obj);
            break;
        case JSI_VT_VARIABLE:
            assert(v->d.lval != v);
            Jsi_DecrRefCount(interp, v->d.lval);
            break;
        case JSI_VT_STRING:
            if (v->d.s.str && !v->f.bits.isstrkey) {
                Jsi_Free(v->d.s.str);
                /*Jsi_HashEntry *hPtr;
                if ((hPtr = Jsi_HashEntryFind(strDebug, v->d.s.str)))
                    Jsi_HashEntryDelete(hPtr);*/
            }
            break;
        default:
            break;
    }
    v->vt = JSI_VT_UNDEF;
}
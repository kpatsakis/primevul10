int Jsi_DecrRefCount(Jsi_Interp* interp, Jsi_Value *v) {
    SIGASSERT(v,VALUE);
    if (v->refCnt<=0) {
#ifdef JSI_MEM_DEBUG
        fprintf(stderr, "Value decr with ref %d: VD.Idx=%d\n", v->refCnt, v->VD.Idx);
#endif
        return -2;
    }
    int ref;
    jsi_DebugValue(v,"Decr", jsi_DebugValueCallIdx(), interp);
    if ((ref = --(v->refCnt)) <= 0) {
        v->refCnt = -1;
        Jsi_ValueFree(interp, v);
    }
    return ref;
}
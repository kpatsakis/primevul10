static void jsiClearThis(Jsi_Interp *interp, int ofs) {
    Jsi_Value **vPtr = &_jsi_THISIDX(ofs), *v = *vPtr;
    if (!v) return;
#ifndef XX_NEWSTACK
    Jsi_ValueReset(interp, vPtr);
#else
    if (v->refCnt<=1)
        Jsi_ValueReset(interp, vPtr);
    else {
        Jsi_DecrRefCount(interp, v);
        _jsi_THISIDX(ofs) = Jsi_ValueNew1(interp);
    }
#endif
}
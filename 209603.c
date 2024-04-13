void Jsi_ValueReset(Jsi_Interp *interp, Jsi_Value **vPtr) {
    Jsi_Value *v = *vPtr;
    SIGASSERTV(v,VALUE);
    Assert(v->vt <= JSI_VT__MAX);
    jsi_DebugValue(v, "Reset", jsi_DebugValueCallIdx(), interp);
    Assert(v->refCnt>=0);
    v->f.bits.lookupfailed = 0; // TODO: rework lookup-fail mechanism.
    if (v->vt == JSI_VT_UNDEF)
        return;
    ValueFree(interp, v);
    v->f.flag = 0;
}
static Jsi_Value *ValueNew(Jsi_Interp *interp)
{
    interp->dbPtr->valueCnt++;
    interp->dbPtr->valueAllocCnt++;
    Jsi_Value *v = (Jsi_Value *)Jsi_Calloc(1,sizeof(*v));
    SIGINIT(v,VALUE)
    v->vt = JSI_VT_UNDEF;
    jsi_DebugValue(v,"New", jsi_DebugValueCallIdx(), interp);
    return v;
}
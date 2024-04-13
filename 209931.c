bool Jsi_IsShared(Jsi_Interp* interp, Jsi_Value *v)
{
    SIGASSERT(v,VALUE);
    return (v->refCnt > 1);
}
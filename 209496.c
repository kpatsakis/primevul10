int Jsi_IncrRefCount(Jsi_Interp* interp, Jsi_Value *v)
{
    SIGASSERT(v,VALUE);
    assert(v->refCnt>=0);
    jsi_DebugValue(v,"Incr", jsi_DebugValueCallIdx(), interp);
    return ++(v->refCnt);
}
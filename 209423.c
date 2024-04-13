void jsi_DebugValue(Jsi_Value* v, const char *reason, uint cidx, Jsi_Interp *interp)
{
    if (jsi_memDebugBreakIdx && jsi_memDebugBreakIdx == v->VD.Idx)
        jsi_memDebugBreak();
    return;
}
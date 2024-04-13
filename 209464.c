void jsi_DebugObj(Jsi_Obj* o, const char *reason, uint cidx, Jsi_Interp *interp)
{
    if (jsi_memDebugBreakIdx && jsi_memDebugBreakIdx == o->VD.Idx)
        jsi_memDebugBreak();
    return;
}
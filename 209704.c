void jsi_PstateFree(jsi_Pstate *ps)
{
    /* TODO: when do we free opcodes */
    jsi_PstateClear(ps);
    Jsi_Free(ps->lexer);
    if (ps->opcodes)
        jsi_FreeOpcodes(ps->opcodes);
    if (ps->hPtr)
        Jsi_HashEntryDelete(ps->hPtr);
    if (ps->argsTbl)
        Jsi_HashDelete(ps->argsTbl);
    if (ps->fastVarTbl)
        Jsi_HashDelete(ps->fastVarTbl);
    if (ps->strTbl)
        Jsi_HashDelete(ps->strTbl);
    if (ps->last_exception)
        Jsi_DecrRefCount(ps->interp, ps->last_exception);
    _JSI_MEMCLEAR(ps);
    Jsi_Free(ps);
}
void jsi_ScopeStrsFree(Jsi_Interp *interp, Jsi_ScopeStrs *ss)
{
    if (!ss) return;
    int i;
    for (i=0; i<ss->count; i++)
        if (ss->args[i].defValue)
            Jsi_DecrRefCount(interp, ss->args[i].defValue);
    if (ss->args)
        Jsi_Free(ss->args);
    Jsi_Free(ss);
}
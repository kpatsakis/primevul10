static Jsi_ScopeStrs *jsi_ScopeStrsDup(jsi_Pstate *ps, Jsi_ScopeStrs *ss)
{
    Jsi_ScopeStrs *n = jsi_ScopeStrsNew();
    int i;
    if (!ss) return n;
    *n = *ss;
    if (!ss->args) return n;
    n->args = (jsi_ArgValue*)Jsi_Calloc(n->count, sizeof(ss->args[0]));
    n->_size = n->count;
    memcpy(n->args, ss->args, (n->count *  sizeof(ss->args[0])));
    for (i = 0; i < ss->count; ++i) {
        if (ss->args[i].defValue)
            Jsi_IncrRefCount(ps->interp, ss->args[i].defValue);
    }
    return n;
}
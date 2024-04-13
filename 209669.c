Jsi_ScopeStrs *jsi_ScopeGetVarlist(jsi_Pstate *ps)
{
    Jsi_Interp *interp = ps->interp;
    return jsi_ScopeStrsDup(ps, interp->scopes[interp->cur_scope]);
}
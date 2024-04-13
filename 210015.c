void jsi_PstateAddVar(jsi_Pstate *ps, jsi_Pline *line, const char *str)
{
    Jsi_Interp *interp = ps->interp;
    int i;
    if (interp->scopes[interp->cur_scope] == NULL)
        interp->scopes[interp->cur_scope] = (Jsi_ScopeStrs *)jsi_ScopeStrsNew();
    
    for (i = 0; i < interp->scopes[interp->cur_scope]->count; ++i) {
        if (Jsi_Strcmp(str, interp->scopes[interp->cur_scope]->args[i].name) == 0) {
            Jsi_Interp *interp = ps->interp;
            if (interp && interp->strict) {
                interp->parseLine = line;
                Jsi_LogWarn("duplicate var: %s", str);
                interp->parseLine = NULL;
            }
            return;
        }
    }
    jsi_ScopeStrsPush(ps->interp, interp->scopes[interp->cur_scope], str, JSI_VT_UNDEF);
}
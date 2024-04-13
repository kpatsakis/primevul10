void jsi_PstatePop(jsi_Pstate *ps)
{
    Jsi_Interp *interp = ps->interp;
    if (interp->cur_scope <= 0) {
        Jsi_LogBug("No more scope to pop");
        return;
    }
    jsi_ScopeStrsFree(interp, interp->scopes[interp->cur_scope]);
    interp->scopes[interp->cur_scope] = NULL;
    interp->cur_scope--;
}
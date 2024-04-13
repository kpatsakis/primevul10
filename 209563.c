static int jsi_do_throw(Jsi_Interp *interp, jsi_Pstate *ps, jsi_OpCode **ipp, jsi_TryList **tlp,
     jsi_ScopeChain **scope, Jsi_Value **currentScope, int *context_id, Jsi_Value *top, const char *nam) {
    if (Jsi_InterpGone(interp))
        return JSI_ERROR;
    jsi_TryList *trylist = *tlp;
    while (1) {
        if (trylist == NULL) {
            const char *str = (top?Jsi_ValueString(interp, top, NULL):"");
            if (str) {
                if (!Jsi_Strcmp(nam, "help"))
                    Jsi_LogError("...%s", str);
                else
                    Jsi_LogError("%s: %s", nam, str);
            }
            return JSI_ERROR;
        }
        if (trylist->type == jsi_TL_TRY) {
            int n = interp->framePtr->Sp - trylist->d.td.tsp;
            jsiPop(interp, n);
            if (*ipp >= trylist->d.td.tstart && *ipp < trylist->d.td.tend) {
                *ipp = trylist->d.td.cstart - 1;
                break;
            } else if (*ipp >= trylist->d.td.cstart && *ipp < trylist->d.td.cend) {
                trylist->d.td.last_op = jsi_LOP_THROW;
                *ipp = trylist->d.td.fstart - 1;
                break;
            } else if (*ipp >= trylist->d.td.fstart && *ipp < trylist->d.td.fend) {
                jsi_pop_try(interp, tlp);
                trylist = *tlp;
            } else Jsi_LogBug("Throw within a try, but not in its scope?");
        } else {
            jsi_restore_scope(interp, ps, trylist, scope, currentScope, context_id);
            jsi_pop_try(interp, tlp);
            trylist = *tlp;
        }
    }
    return JSI_OK;
}
static void jsi_restore_scope(Jsi_Interp* interp, jsi_Pstate *ps, jsi_TryList* trylist,
  jsi_ScopeChain **scope, Jsi_Value **currentScope, int *context_id) {

/* JSI_RESTORE_SCOPE(scope_save, curscope_save)*/
    if (*scope != (trylist->scope_save)) {
        jsi_ScopeChainFree(interp, *scope);
        *scope = (trylist->scope_save);
        interp->framePtr->ingsc = *scope;
    }
    if (*currentScope != (trylist->curscope_save)) {
        Jsi_DecrRefCount(interp, *currentScope);
        *currentScope = (trylist->curscope_save); 
        interp->framePtr->incsc = *currentScope;
    }
    *context_id = ps->_context_id++; 
}
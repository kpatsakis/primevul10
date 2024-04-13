static jsi_TryList *jsi_trylist_new(jsi_try_op_type t, jsi_ScopeChain *scope_save, Jsi_Value *curscope_save)
{
    jsi_TryList *n = (jsi_TryList *)Jsi_Calloc(1,sizeof(*n));
    
    n->type = t;
    n->curscope_save = curscope_save;
    /*Jsi_IncrRefCount(interp, curscope_save);*/
    n->scope_save = scope_save;
    
    return n;
}
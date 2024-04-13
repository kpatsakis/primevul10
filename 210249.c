static Jsi_RC jsi_AliasFree(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *data) {
    /* TODO: deal with other copies of func may be floating around (refCount). */
    AliasCmd *ac = (AliasCmd *)data;
    if (!ac) return JSI_ERROR;
    SIGASSERT(ac,ALIASCMD);
    if (ac->func)
        Jsi_DecrRefCount(ac->dinterp, ac->func);
    if (ac->args)
        Jsi_DecrRefCount(ac->dinterp, ac->args);
    if (!ac->cmdVal)
        return JSI_OK;
    Jsi_Func *fobj = ac->cmdVal->d.obj->d.fobj->func;
    fobj->cmdSpec->reserved[2] = NULL;
    fobj->cmdSpec->proc = NULL;
    if (ac->intobj && ac->intobj->subinterp) {
        Jsi_CommandDelete(ac->intobj->subinterp, ac->cmdName);
        //if (Jsi_Strchr(ac->cmdName, '.'))
        //    Jsi_LogBug("alias free with X.Y dot name leaks memory: %s", ac->cmdName);
    } else
        Jsi_DecrRefCount(ac->subinterp, ac->cmdVal);
    _JSI_MEMCLEAR(ac);
    Jsi_Free(ac);
    return JSI_OK;
}
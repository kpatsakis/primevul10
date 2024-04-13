void jsi_FuncObjFree(Jsi_FuncObj *fobj)
{
    if (fobj->scope)
        jsi_ScopeChainFree(fobj->interp, fobj->scope);
    if (fobj->bindArgs)
        Jsi_DecrRefCount(fobj->interp, fobj->bindArgs);
    if (fobj->bindFunc)
        Jsi_DecrRefCount(fobj->interp, fobj->bindFunc);
    if (fobj->func)
        jsi_FuncFree(fobj->interp, fobj->func);
    _JSI_MEMCLEAR(fobj);
    Jsi_Free(fobj);
}
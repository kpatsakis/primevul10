static Jsi_RC freeFuncObjTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_Obj *v = (Jsi_Obj *)ptr;
    if (!v) return JSI_OK;
    SIGASSERT(v,OBJ);
    if (v->ot != JSI_OT_FUNCTION)
        fprintf(stderr, "invalid func obj\n");
    else if (v->d.fobj) {
        if (v->d.fobj->scope) {
            jsi_ScopeChain *scope = v->d.fobj->scope;
            v->d.fobj->scope = NULL;
            jsi_ScopeChainFree(interp, scope);
        }
    }
    Jsi_ObjDecrRefCount(interp, v);
    return JSI_OK;
}
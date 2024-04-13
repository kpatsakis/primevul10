static Jsi_RC freeBindObjTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_Obj *v = (Jsi_Obj *)ptr;
    if (!v) return JSI_OK;
    SIGASSERT(v,OBJ);
    if (v->ot != JSI_OT_FUNCTION)
        fprintf(stderr, "invalid func obj\n");
    else if (v->d.fobj && v->d.fobj->scope) {
        v->d.fobj->scope = NULL;
    }
    Jsi_ObjDecrRefCount(interp, v);
    return JSI_OK;
}
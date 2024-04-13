static Jsi_RC freeGenObjTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_Obj *obj = (Jsi_Obj *)ptr;
    SIGASSERT(obj,OBJ);
    if (!obj) return JSI_OK;
    Jsi_ObjDecrRefCount(interp, obj);
    return JSI_OK;
}
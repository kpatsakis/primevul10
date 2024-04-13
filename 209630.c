static int fastVarFree(Jsi_Interp *interp, void *ptr) {
    FastVar *fv = ptr;
    Jsi_Value *v = fv->var.lval;
    if (v) {
        //printf("FV FREE: %p (%d/%d)\n", fv, v->refCnt, v->vt == JSI_VT_OBJECT?v->d.obj->refcnt:-99);
        //Jsi_DecrRefCount(interp, v);
    }
    return JSI_OK;
}
static Jsi_RC freeFuncsTbl(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    Jsi_Func *func = (Jsi_Func *)ptr;
    if (!func) return JSI_OK;
    SIGASSERT(func,FUNC);
    func->hPtr = NULL;
    jsi_FuncFree(interp, func);
    return JSI_OK;
}
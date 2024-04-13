bool jsi_FuncIsNoop(Jsi_Interp* interp, Jsi_Value *func) {
    Jsi_FuncObj *funcPtr;
    Jsi_Func *f;
    if (func->vt != JSI_VT_OBJECT || func->d.obj->ot != JSI_OT_FUNCTION)
        return 0;
    funcPtr = func->d.obj->d.fobj;
    f = funcPtr->func;
    return (f->callback == jsi_NoOpCmd);
}
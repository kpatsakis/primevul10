Jsi_RC Jsi_FunctionArguments(Jsi_Interp *interp, Jsi_Value *func, int *argcPtr)
{
    Jsi_FuncObj *funcPtr;
    Jsi_Func *f;
    if (!Jsi_ValueIsFunction(interp, func))
        return JSI_ERROR;
    funcPtr = func->d.obj->d.fobj;
    f = funcPtr->func;
    SIGASSERT(f, FUNC);
    *argcPtr = f->argnames->count;
    return JSI_OK;
}
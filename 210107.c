Jsi_FuncObj *jsi_FuncObjNew(Jsi_Interp *interp, Jsi_Func *func)
{
    Jsi_FuncObj *f = (Jsi_FuncObj*)Jsi_Calloc(1,sizeof(Jsi_FuncObj));
    f->interp = interp;
    SIGINIT(f,FUNCOBJ);
    f->func = func;
    func->refCnt++;
    return f;
}
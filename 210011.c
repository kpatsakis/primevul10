Jsi_Func *jsi_FuncNew(Jsi_Interp *interp)
{
     Jsi_Func *func = (Jsi_Func*)Jsi_Calloc(1, sizeof(Jsi_Func));
     SIGINIT(func, FUNC);
     func->hPtr = Jsi_HashSet(interp->funcsTbl, func, func);
     func->refCnt = 1;
     interp->funcCnt++;
     return func;
}
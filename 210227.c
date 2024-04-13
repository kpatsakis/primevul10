static Jsi_RC jsi_FunctionInvoke(Jsi_Interp *interp, Jsi_Value *tocall, Jsi_Value *args, Jsi_Value **ret, Jsi_Value *_this)
{
    if (interp->maxDepth>0 && interp->maxDepth && interp->callDepth>=interp->maxDepth)
        return Jsi_LogError("max call depth exceeded");
    if (interp->deleting)
        return JSI_ERROR;
    if (!Jsi_ValueIsFunction(interp, tocall)) 
        return Jsi_LogError("can not execute expression, expression is not a function");
    if (!tocall->d.obj->d.fobj) {   /* empty function */
        return JSI_OK;
    }
    if (!ret) {
        if (!interp->nullFuncRet) {
            interp->nullFuncRet = Jsi_ValueNew(interp);
            Jsi_IncrRefCount(interp, interp->nullFuncRet);
        }
        ret = &interp->nullFuncRet;
        Jsi_ValueMakeUndef(interp, ret);
    }
    if (!args) {
        if (!interp->nullFuncArg) {
            interp->nullFuncArg = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, NULL, 0, 0));
            Jsi_IncrRefCount(interp, interp->nullFuncArg);
        }
        args = interp->nullFuncArg;
    }
    /* func to call */
    Jsi_Func *funcPtr = tocall->d.obj->d.fobj->func;
    SIGASSERT(funcPtr, FUNC);
    
    /* prepare args */
    if (args->vt != JSI_VT_OBJECT || !Jsi_ObjIsArray(interp, args->d.obj)) 
        return Jsi_LogError("argument must be an array");
    /* new this */
    Jsi_Value *fthis = Jsi_ValueDup(interp, _this ? _this : tocall);
    Jsi_Func *prevActive = interp->activeFunc;
    Jsi_RC res = jsi_SharedArgs(interp, args, funcPtr, 1);
    bool isalloc = 0;
    int calltrc = 0;
    int tc = interp->traceCall | (funcPtr->pkg?funcPtr->pkg->popts.modConf.traceCall:0);
    interp->callDepth++;
    if (res == JSI_OK) {
        jsi_InitLocalVar(interp, args, funcPtr);
        jsi_SetCallee(interp, args, tocall);
        isalloc = 1;
        Jsi_IncrRefCount(interp, args);
        if (funcPtr->type == FC_NORMAL) {
            if ((tc&jsi_callTraceFuncs) && funcPtr->name)
                calltrc = 1;
        } else {
            if ((tc&jsi_callTraceCmds) && funcPtr->name)
                calltrc = 1;
        }
        interp->activeFunc = funcPtr;
        if (funcPtr->type == FC_NORMAL) {
            if (calltrc)
                jsi_TraceFuncCall(interp, funcPtr, NULL, fthis, args, NULL, tc);
            res = jsi_evalcode(interp->ps, funcPtr, funcPtr->opcodes, tocall->d.obj->d.fobj->scope, 
                args, fthis, ret);
        } else {
            if (calltrc)
                jsi_TraceFuncCall(interp, funcPtr, NULL, fthis, args, NULL, tc);
            res = funcPtr->callback(interp, args, fthis, ret, funcPtr);
        }
        funcPtr->callCnt++;
    }
    interp->callDepth--;
    if (res == JSI_OK && funcPtr->retType)
        res = jsi_ArgTypeCheck(interp, funcPtr->retType, *ret, "returned from", funcPtr->name, 0, funcPtr, 0);
    if (calltrc && (tc&jsi_callTraceReturn))
        jsi_TraceFuncCall(interp, funcPtr, NULL, fthis, NULL, *ret, tc);
    interp->activeFunc = prevActive;
    jsi_SharedArgs(interp, args, funcPtr, 0);
    if (isalloc) 
        Jsi_DecrRefCount(interp, args);
    Jsi_DecrRefCount(interp, fthis);
    return res;
}
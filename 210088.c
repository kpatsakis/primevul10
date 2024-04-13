Jsi_RC jsi_evalcode(jsi_Pstate *ps, Jsi_Func *func, Jsi_OpCodes *opcodes, 
         jsi_ScopeChain *scope, Jsi_Value *fargs,
         Jsi_Value *_this,
         Jsi_Value **vret)
{
    Jsi_Interp *interp = ps->interp;
    if (interp->exited)
        return JSI_ERROR;
    Jsi_RC rc;
    jsi_Frame frame = *interp->framePtr;
    frame.parent = interp->framePtr;
    interp->framePtr = &frame;
    frame.parent->child = interp->framePtr = &frame;
    frame.ps = ps;
    frame.ingsc = scope;
    frame.incsc = fargs;
    frame.inthis = _this;
    frame.opcodes = opcodes;
    frame.fileName = ((func && func->script)?func->script:interp->curFile);
    frame.funcName = interp->curFunction;
    frame.dirName = interp->curDir;
    if (frame.fileName && frame.fileName == frame.parent->fileName)
        frame.logflag = frame.parent->logflag;
    else
        frame.logflag = 0;
    frame.level = frame.parent->level+1;
    frame.evalFuncPtr = func;
    frame.arguments = NULL;
   // if (func && func->strict)
    //    frame.strict = 1;
    if (interp->curIp)
        frame.parent->line = interp->curIp->Line;
    frame.ip = interp->curIp;
    interp->refCount++;
    interp->level++;
    Jsi_IncrRefCount(interp, fargs);
    rc = jsi_evalcode_sub(ps, opcodes, scope, fargs, _this, *vret);
    Jsi_DecrRefCount(interp, fargs);
    if (interp->didReturn == 0 && !interp->exited && rc == JSI_OK) {
        if ((interp->evalFlags&JSI_EVAL_RETURN)==0)
            Jsi_ValueMakeUndef(interp, vret);
        /*if (interp->framePtr->Sp != oldSp) //TODO: at some point after memory refs???
            Jsi_LogBug("Stack not balance after execute script");*/
    }
    if (frame.arguments)
        Jsi_DecrRefCount(interp, frame.arguments);
    interp->didReturn = 0;
    interp->refCount--;
    interp->level--;
    interp->framePtr = frame.parent;
    interp->framePtr->child = NULL;
    interp->curIp = frame.ip;
    if (interp->exited)
        rc = JSI_ERROR;
    return rc;
}
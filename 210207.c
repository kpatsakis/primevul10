Jsi_RC Jsi_FunctionInvoke(Jsi_Interp *interp, Jsi_Value *func, Jsi_Value *args, Jsi_Value **ret, Jsi_Value *_this)
{
    // Arrange for error reporting to point to called function.
    Jsi_Func *fstatic = func->d.obj->d.fobj->func;
    jsi_OpCode *oldops = interp->curIp;
    if (fstatic->opcodes)
        interp->curIp = fstatic->opcodes->codes;
    Jsi_RC rc = jsi_FunctionInvoke(interp, func, args, ret, _this);
    interp->curIp = oldops;
    if (Jsi_InterpGone(interp))
        return JSI_ERROR;
    return rc;
}
static Jsi_RC WebSocketIdConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    return WebSocketIdCmdOp(interp, args, _this, ret, funcPtr, 0);
}
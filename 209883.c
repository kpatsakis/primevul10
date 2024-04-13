static Jsi_RC InterpUplevelCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    return InterpEvalCmd_(interp, args, _this, ret, funcPtr, 1);
}
static Jsi_RC StringTrimRightCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    return _StringTrimCmd(interp, args, _this, ret, funcPtr, 2);
}
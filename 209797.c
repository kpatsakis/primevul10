static Jsi_RC NumberIsFiniteCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr) {
    return jsi_NumberIsFiniteCmd(interp, args, _this, ret, funcPtr, 1);
}
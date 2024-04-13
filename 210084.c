bool Jsi_FunctionReturnIgnored(Jsi_Interp *interp, Jsi_Func *funcPtr) {
    return funcPtr->callflags.bits.isdiscard;
}
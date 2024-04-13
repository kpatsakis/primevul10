char* Jsi_ValueNormalPath(Jsi_Interp *interp, Jsi_Value *file, Jsi_DString *dStr) {
    return Jsi_NormalPath(interp, Jsi_ValueString(interp, file, NULL), dStr);
}
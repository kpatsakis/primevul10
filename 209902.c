Jsi_Value * jsi_ValueDup(Jsi_Interp *interp, Jsi_Value *ov, const char *fname, int line, const char *func) {
    Jsi_Value *v = ValueDup(interp, ov);
    jsi_ValueDebugUpdate(interp, v, valueDebugTbl, fname, line, func);
    return v;
}
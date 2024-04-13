Jsi_Value * jsi_ValueNew1(Jsi_Interp *interp, const char *fname, int line, const char *func) {
    Jsi_Value *v = ValueNew1(interp);
    jsi_ValueDebugUpdate(interp, v, valueDebugTbl, fname, line, func);
    return v;
}
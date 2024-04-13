Jsi_Value* Jsi_ValueNewStringDup(Jsi_Interp *interp, const char *s) {
    return Jsi_ValueNewString(interp, Jsi_Strdup(s), -1);
}
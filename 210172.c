Jsi_Value* Jsi_ValueMakeString(Jsi_Interp *interp, Jsi_Value **vPtr, const char *s) {
    return Jsi_ValueMakeBlob(interp, vPtr, (unsigned char *)s, Jsi_Strlen(s));
}
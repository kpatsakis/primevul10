void Jsi_ValueCopy(Jsi_Interp *interp, Jsi_Value *to, Jsi_Value *from ) {
    return jsi_ValueCopyMove(interp, to, from, 1);
}
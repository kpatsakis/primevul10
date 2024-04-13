void Jsi_ValueMove(Jsi_Interp *interp, Jsi_Value *to, Jsi_Value *from )  {
    return jsi_ValueCopyMove(interp, to, from, 0);
}
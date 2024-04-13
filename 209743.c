int Jsi_Getc(Jsi_Interp *interp, Jsi_Channel chan) {
    if (chan->fsPtr==0 || !chan->fsPtr->getcProc) return -1;
    return chan->fsPtr->getcProc(chan);
}
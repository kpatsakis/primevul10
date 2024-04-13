int Jsi_Ungetc(Jsi_Interp *interp, Jsi_Channel chan, int ch) {
    if (chan->fsPtr==0 || !chan->fsPtr->ungetcProc) return -1;
    return chan->fsPtr->ungetcProc(chan, ch);
}
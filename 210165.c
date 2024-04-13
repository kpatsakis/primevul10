int Jsi_Write(Jsi_Interp *interp, Jsi_Channel chan, const char *bufPtr, int slen) {
    if (chan->fsPtr==0 || !chan->fsPtr->writeProc) return -1;
    return chan->fsPtr->writeProc(chan, bufPtr, slen);
}
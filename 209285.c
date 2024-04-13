int Jsi_Flush(Jsi_Interp *interp, Jsi_Channel chan) {
    if (chan->fsPtr==0 || !chan->fsPtr->flushProc) return -1;
    return chan->fsPtr->flushProc(chan);
}
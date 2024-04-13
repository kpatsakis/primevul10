int Jsi_Truncate(Jsi_Interp *interp, Jsi_Channel chan, unsigned int len) {
    if (chan->fsPtr==0 || !chan->fsPtr->truncateProc) return -1;
    return chan->fsPtr->truncateProc(chan, len);
}
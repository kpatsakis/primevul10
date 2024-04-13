Jsi_Wide Jsi_Rewind(Jsi_Interp *interp, Jsi_Channel chan) {
    if (chan->fsPtr==0 || !chan->fsPtr->rewindProc) return -1;
    return chan->fsPtr->rewindProc(chan);
}
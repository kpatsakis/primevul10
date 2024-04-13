Jsi_Wide Jsi_Tell(Jsi_Interp *interp, Jsi_Channel chan) {
    if (chan->fsPtr==0 || !chan->fsPtr->tellProc) return -1;
    return chan->fsPtr->tellProc(chan);
}
Jsi_Wide Jsi_Seek(Jsi_Interp *interp, Jsi_Channel chan, Jsi_Wide offset, int mode) {
    if (chan->fsPtr==0 || !chan->fsPtr->seekProc) return -1;
    return chan->fsPtr->seekProc(chan, offset, mode);
}
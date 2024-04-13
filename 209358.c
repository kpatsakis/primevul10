int Jsi_Eof(Jsi_Interp *interp, Jsi_Channel chan) {
    if (chan->fsPtr==0 || !chan->fsPtr->eofProc) return -1;
    return chan->fsPtr->eofProc(chan);
}
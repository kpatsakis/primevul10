int Jsi_Read(Jsi_Interp *interp, Jsi_Channel chan, char *bufPtr, int toRead) {
    if (chan->fsPtr==0 || !chan->fsPtr->readProc) return -1;
    return chan->fsPtr->readProc(chan, bufPtr, toRead);
}
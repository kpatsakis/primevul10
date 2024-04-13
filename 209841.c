char * Jsi_Gets(Jsi_Interp *interp, Jsi_Channel chan, char *s, int size) {
    if (chan->fsPtr==0 || !chan->fsPtr->getsProc) return NULL;
    return chan->fsPtr->getsProc(chan, s, size);
}
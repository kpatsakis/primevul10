int Jsi_Close(Jsi_Interp *interp, Jsi_Channel chan) {
    if (chan->fsPtr==0 || !chan->fsPtr->closeProc) return -1;
    if (chan->flags&JSI_FS_NOCLOSE) return -1;
    int rc = chan->fsPtr->closeProc(chan);
    if (rc == 0)
        Jsi_Free(chan);
    return rc;
}
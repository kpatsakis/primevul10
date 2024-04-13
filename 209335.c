Jsi_Channel Jsi_GetStdChannel(Jsi_Interp *interp, int id) {
    if (id<0 || id>2)
        return NULL;
    return jsiIntData.stdChans+id;
}
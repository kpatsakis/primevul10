static int jsi_FSWriteProc(Jsi_Channel chan, const char *buf, int size) {
    return fwrite(buf, 1, size, _JSI_GETFP(chan,0));
}
static int jsi_FSReadProc(Jsi_Channel chan, char *buf, int size) {
    return fread(buf, 1, size, _JSI_GETFP(chan,0));
}
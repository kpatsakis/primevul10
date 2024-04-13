static int jsi_FSUngetcProc(Jsi_Channel chan, int ch) {
    return ungetc(ch, _JSI_GETFP(chan,1));
}
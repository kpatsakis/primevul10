static int jsi_FSPutsProc(Jsi_Channel chan, const char *s) {
    return fputs(s, _JSI_GETFP(chan,0));
}
static char * jsi_FSGetsProc(Jsi_Channel chan, char *s, int size) {
    return fgets(s, size, _JSI_GETFP(chan,1));
}
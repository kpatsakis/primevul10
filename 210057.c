static int jsi_FSGetcProc(Jsi_Channel chan) {
    return fgetc(_JSI_GETFP(chan,1));
}
static int jsi_FSSeekProc(Jsi_Channel chan, Jsi_Wide offset, int mode) { return fseek(_JSI_GETFP(chan,1), offset, mode);}
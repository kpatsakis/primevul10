static int jsi_FSCloseProc(Jsi_Channel chan) { return fclose(_JSI_GETFP(chan,1));}
Jsi_Channel jsi_FSOpenProc(Jsi_Interp *interp, Jsi_Value *file, const char *modeString)
{
    return Jsi_Open(interp, file, modeString);
}
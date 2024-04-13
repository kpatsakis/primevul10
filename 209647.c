Jsi_OptionsCustomPrint(void* clientData, Jsi_Interp *interp, const char *name, void *rec, int offset)
{
    char *record = (char*)rec;
    Jsi_Value *valuePtr;
    valuePtr = *(Jsi_Value **)(record + offset);
    return valuePtr;
}
Jsi_ValueGetIndex( Jsi_Interp *interp, Jsi_Value *valPtr,
    const char **tablePtr, const char *msg, int flags, int *indexPtr)
{
    char *val = Jsi_ValueString(interp, valPtr, NULL);
    if (val == NULL) 
        return Jsi_LogError("expected string");
    return Jsi_GetIndex(interp, val, tablePtr, msg, flags, indexPtr);
}
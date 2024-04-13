Jsi_RC Jsi_GetIntFromValue(Jsi_Interp* interp, Jsi_Value *value, int *n)
{
    if (!Jsi_ValueIsNumber(interp, value)) 
        return Jsi_LogError("invalid number");
    return Jsi_GetIntFromValueBase(interp, value, n, 0, 0);
}
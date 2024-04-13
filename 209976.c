Jsi_RC Jsi_GetIntFromValueBase(Jsi_Interp* interp, Jsi_Value *value, int *n, int base, int flags)
{
    int noMsg = (flags & JSI_NO_ERRMSG);
    /* TODO: inefficient to convert to double then back. */
    if (!value)
        return JSI_ERROR;
    Jsi_Number d = Jsi_ValueToNumberInt(interp, value, 1);
    if (!Jsi_NumberIsFinite(d))
    {
        if (!noMsg)
            Jsi_LogError("invalid number");
        return JSI_ERROR;
    }
    Jsi_ValueReset(interp,&value);
    Jsi_ValueMakeNumber(interp, &value, d);
    *n = (int)d;
    return JSI_OK;
}
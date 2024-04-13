static Jsi_RC JSONStringifyCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    bool strict = 1;
    Jsi_Value *jsv = Jsi_ValueArrayIndex(interp, args, 1);
    if (jsv && Jsi_ValueGetBoolean(interp, jsv, &strict) != JSI_OK) 
        return Jsi_LogError("Expected boolean");
    int quote = JSI_OUTPUT_JSON;
    if (strict) quote|=JSI_JSON_STRICT;
    Jsi_DString dStr = {};
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_ValueGetDString(interp, arg, &dStr, quote);
    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
}
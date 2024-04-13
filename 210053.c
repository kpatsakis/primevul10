const char* Jsi_ValueGetDString(Jsi_Interp *interp, Jsi_Value* v, Jsi_DString *dStr, int quote)
{
    objwalker ow;
    ow.quote = quote;
    ow.depth = 0;
    ow.dStr = dStr;
    jsiValueGetString(interp, v, dStr, &ow);
    return Jsi_DSValue(dStr);
}
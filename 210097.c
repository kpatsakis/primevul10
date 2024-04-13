static Jsi_RC jsi_VerifyToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *dStr, void *record, Jsi_Wide flags)
{
    if (dStr)
        return JSI_ERROR;
    Jsi_Value **s = (Jsi_Value**)(((char*)record) + spec->offset);
    if (*s)
        Jsi_ValueReplace(interp, outValue, *s);
    return JSI_OK;
}
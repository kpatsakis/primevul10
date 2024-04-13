static Jsi_RC jsi_NullToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    Jsi_ValueMakeNull(interp, outValue);
    return JSI_OK;
}
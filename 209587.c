static Jsi_RC jsi_SuboptToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    if (outStr)
        return JSI_ERROR;
    char *s = ((char*)record) + spec->offset;
    Jsi_OptionSpec *subspec = (Jsi_OptionSpec *)spec->data;
    if (spec == subspec) 
        return Jsi_LogError("recursive subspec not allowed");
    if (!subspec) 
        return Jsi_LogError("custom suboption spec did not set data: %s", spec->name);
    return Jsi_OptionsDump(interp, subspec, s, outValue, flags);
}
static Jsi_RC jsi_ParentFuncToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    if (outStr)
        return JSI_ERROR;
    Jsi_Value **v = (Jsi_Value **)(((char*)record) + spec->offset);
    //const char *s, *subspec = (const char *)spec->data;
    if (!*v) {
        Jsi_ValueMakeNull(interp, outValue);
        return JSI_OK;
    }
    if (!interp->parent)
        return Jsi_LogError("no parent interp");
    if (spec->data) {
        Jsi_DString dStr = {};
        Jsi_DSPrintf(&dStr, "func(%s)", (char*)spec->data);
        Jsi_ValueFromDS(interp, &dStr, outValue);
    }
    return JSI_OK;
}
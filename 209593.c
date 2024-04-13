static Jsi_RC dbIsNumArray(Jsi_Interp *interp, Jsi_Value *value, Jsi_OptionSpec* spec, void *record)
{
    if (!Jsi_ValueIsArray(interp, value)) 
        return Jsi_LogError("expected array of numbers");
    int i, argc = Jsi_ValueGetLength(interp, value);
    for (i=0; i<argc; i++) {
        Jsi_Value *v = Jsi_ValueArrayIndex(interp, value, i);
        if (!Jsi_ValueIsNumber(interp, v)) 
            return Jsi_LogError("expected array of numbers");
    }
    return JSI_OK;
}
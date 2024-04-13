static Jsi_RC jsi_ValueToSubopt(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    if (inStr)
        return JSI_ERROR;
    char *s = ((char*)record) + spec->offset;
    Jsi_OptionSpec *subspec = (Jsi_OptionSpec *)spec->data;
  
    if (spec == subspec) 
        return Jsi_LogError("subspec was recursive");
    if (!subspec) 
        return Jsi_LogError("custom suboption spec did not set data: %s", spec->name);
    if (inValue && Jsi_ValueIsNull(interp, inValue) == 0 &&
        (Jsi_ValueIsObjType(interp, inValue, JSI_OT_OBJECT)==0 || inValue->d.obj->isarrlist)) 
        return Jsi_LogError("expected object");
    return (Jsi_OptionsProcess(interp, subspec, s, inValue, flags)<0 ? JSI_ERROR : JSI_OK);
}
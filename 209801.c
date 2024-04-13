Jsi_OptionsConf(Jsi_Interp *interp, Jsi_OptionSpec *specs,  void *rec, Jsi_Value *val, Jsi_Value **ret, Jsi_Wide flags)
{
    flags |= JSI_OPTS_IS_UPDATE;
    if (!Jsi_OptionsValid(interp, specs))
        return Jsi_LogError("invalid options");
    
    if (!val)
        return Jsi_OptionsDump(interp, specs, rec, ret, flags);
    if (val->vt == JSI_VT_NULL)
        return Jsi_OptionsDump(interp, specs, rec, ret, flags|JSI_OPTS_VERBOSE);
    if (Jsi_ValueIsString(interp, val)) {
        const char *cp = Jsi_ValueString(interp, val, NULL);
        if (cp && *cp)
            return Jsi_OptionsGet(interp, specs, rec, cp, ret, flags);
        Jsi_Obj *sobj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
        Jsi_Value *svalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        jsi_DumpOptionSpecs(interp, sobj, specs);
        Jsi_ValueReplace(interp, ret, svalue);
        return JSI_OK;
    }
    if (val->vt != JSI_VT_OBJECT) 
        return Jsi_LogError("expected string, object, or null");
    if (Jsi_OptionsProcess(interp, specs, rec, val, JSI_OPTS_IS_UPDATE|flags) < 0)
        return JSI_ERROR;
    return JSI_OK;
}
static Jsi_RC jsi_ValueToParentFunc(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    if (inStr)
        return JSI_ERROR;
    Jsi_Value *val;
    Jsi_Value **v = (Jsi_Value **)(((char*)record) + spec->offset);
    const char *s, *subspec = (const char *)spec->data;
    Jsi_Interp *pinterp = interp->parent;
    if (!pinterp)
        return Jsi_LogError("no parent interp");
    if (!subspec) 
        return Jsi_LogError("custom parentfunc spec did not set data: %s", spec->name);
    if (!inValue || Jsi_ValueIsNull(interp, inValue)) {
        if (*v) Jsi_DecrRefCount(pinterp, *v);
        *v = NULL;
        return JSI_OK;
    }
    if (!(s=Jsi_ValueString(interp, inValue, NULL))) {
        return Jsi_LogError("expected string or null");
    }
    val = Jsi_NameLookup(pinterp, s);
    if (!val)
        return Jsi_LogError("value not found in parent: %s", s);
    if (!Jsi_ValueIsFunction(pinterp, val))
        return Jsi_LogError("expected a func value");
    if (spec->data && (interp->strict || pinterp->strict))
        if (!jsi_FuncIsNoop(pinterp, val)
            && !jsi_FuncArgCheck(pinterp, val->d.obj->d.fobj->func, (char*)spec->data)) 
            return Jsi_LogError("failed setting func pointer for %s", spec->name);

    *v = val;
    Jsi_IncrRefCount(pinterp, val);
    return JSI_OK;
}
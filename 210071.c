Jsi_OptionsDump(Jsi_Interp *interp, Jsi_OptionSpec *specs, void *rec, Jsi_Value **ret, Jsi_Wide flags)
{
    char *record = (char*)rec;
    Jsi_OptionSpec *specPtr = specs;
    int len = 0, i = 0, count = 0;
    if (!Jsi_OptionsValid(interp, specs))
        return Jsi_LogError("invalid options");
    
    while (specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name) {
        specPtr++;
        len+=2;
    }
    if (!len)
        return JSI_OK;
    Jsi_Value *rv = Jsi_ValueMakeObject(interp, NULL, NULL);
    Jsi_IncrRefCount(interp, rv);
    Jsi_Obj *obj = rv->d.obj;
    specPtr = specs;
    while (specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name) {
        Jsi_Value  *vv = Jsi_ValueNew1(interp);
        if (jsi_GetOption(interp, specPtr, record, NULL, &vv, flags) != JSI_OK) {
            Jsi_DecrRefCount(interp, vv);
            Jsi_DecrRefCount(interp, rv);
            return JSI_ERROR;
        }
        if (flags&JSI_OPTS_VERBOSE) {
            // dump: id,value,help,info,init
            Jsi_Value *vrv = Jsi_ValueMakeObject(interp, NULL, NULL);
            Jsi_IncrRefCount(interp, vrv);
            Jsi_Obj *vobj = vrv->d.obj;
            Jsi_ObjInsert(interp, vobj, "value", vv, 0);
            Jsi_DecrRefCount(interp, vv);
            vv = vrv;
            jsi_DumpOptionSpec(interp, vobj, specPtr, 0);
        }
        Jsi_ObjInsert(interp, obj, specPtr->name, vv, 0);
        Jsi_DecrRefCount(interp, vv);
        count++;
        i++;
        specPtr++;
    }
    assert(specPtr->id == JSI_OPTION_END);
    Jsi_ValueReplace(interp, ret, rv);
    Jsi_DecrRefCount(interp, rv);
    return JSI_OK;
}
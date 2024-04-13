Jsi_OptionsProcess(Jsi_Interp *interp, Jsi_OptionSpec *specs,  void *rec, Jsi_Value *args, Jsi_Wide flags)
{
    Jsi_OptionSpec *specPtr;
    int count = 0;
    char *record = (char*)rec;
    Jsi_TreeEntry *tPtr;
    Jsi_TreeSearch search;
    Jsi_Obj *to;
    bool isSafe = interp->isSafe;
    if (!Jsi_OptionsValid(interp, specs))
        return -1;

    if (interp->subOpts.compat && !(flags&JSI_OPTS_FORCE_STRICT))
        flags |=  JSI_OPTS_IGNORE_EXTRA;
    specs = jsi_GetCachedOptionSpecs(interp, specs);
    
    if (args == NULL || args->vt == JSI_VT_NULL) {
        for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name; specPtr++) {
            specPtr->flags &= ~JSI_OPT_IS_SPECIFIED;
            if (jsi_SetOption(interp, specPtr, (char*)specPtr->name, record, NULL, flags, isSafe) != JSI_OK)
                return -1;
        }
        assert(specPtr->id == JSI_OPTION_END);
        return 0;
    }
    if (args->vt != JSI_VT_OBJECT || args->d.obj->ot != JSI_OT_OBJECT || args->d.obj->arr) {
        Jsi_LogError("expected object");
        return -1;
    }
    to = args->d.obj;
    int reqCnt = 0;
    if ((JSI_OPTS_IS_UPDATE&flags)==0 && !(JSI_OPT_PASS2&flags)) {
        for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name; specPtr++) {
            specPtr->flags &= ~JSI_OPT_IS_SPECIFIED;
            if (specPtr->flags &  JSI_OPT_REQUIRED)
                reqCnt++;
        }
        assert(specPtr->id == JSI_OPTION_END);
    }
        
    for (tPtr = Jsi_TreeSearchFirst(to->tree, &search, 0, NULL);
        tPtr != NULL; tPtr = Jsi_TreeSearchNext(&search)) {
        
        const char *arg;
        Jsi_Value *optval;
        count++;
        arg =(char*) Jsi_TreeKeyGet(tPtr);
        optval = (Jsi_Value*)Jsi_TreeValueGet(tPtr);

        specPtr = Jsi_OptionsFind(interp, specs, arg, flags);
        if (specPtr == NULL) {
            if (flags&JSI_OPTS_IGNORE_EXTRA)
                continue;
            count = -1;
            goto done;
        }
        if (((JSI_OPT_PASS2&flags) && !(JSI_OPT_PASS2&specPtr->flags))
            || ((JSI_OPT_PASS2&specPtr->flags) && !(JSI_OPT_PASS2&flags)))
            continue;
            
        if ((JSI_OPT_READ_ONLY&specPtr->flags)) {
            Jsi_LogError("Error option is readonly: \"%.40s\"", specPtr->name);
            count = -1;
            goto done;
        }
        if ((JSI_OPTS_IS_UPDATE&flags) && (JSI_OPT_INIT_ONLY&specPtr->flags)) {
            Jsi_LogError("Error can not update option: \"%.40s\"", specPtr->name);
            count = -1;
            goto done;
        }
        if (isSafe && (JSI_OPTS_IS_UPDATE&flags) && (JSI_OPT_LOCKSAFE&specPtr->flags)) {
            Jsi_LogError("Error isSafe disallows updating option: \"%.40s\"", specPtr->name);
            count = -1;
            goto done;
        }

        if (jsi_SetOption(interp, specPtr, (char*)arg, record, optval, flags, isSafe) != JSI_OK) {
            count = -1;
            goto done;
        }
 
        specPtr->flags |= JSI_OPT_IS_SPECIFIED;
    }
    if (reqCnt) {
        for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name; specPtr++) {
            if (specPtr->flags &  JSI_OPT_REQUIRED &&
                !(specPtr->flags&JSI_OPT_IS_SPECIFIED)) {

                Jsi_LogError("Error required field not specified: \"%.40s\"", specPtr->name);
                count = -1;
            }
        }
    }

done:
    Jsi_TreeSearchDone(&search);
    if (count<0 && !(JSI_OPTS_IS_UPDATE&flags))
        Jsi_OptionsFree(interp, specs, rec, flags);
    return count;
}
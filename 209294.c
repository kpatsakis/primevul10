Jsi_OptionsSet(Jsi_Interp *interp, Jsi_OptionSpec *specs, void* rec, const char *option, Jsi_Value *valuePtr, Jsi_Wide flags)
{
    char *record = (char*)rec;
    Jsi_OptionSpec *specPtr;
    specs = jsi_GetCachedOptionSpecs(interp, specs);
    const char *cp = NULL, *cb = NULL;
    bool isSafe = interp->isSafe;
    if (option) {
        cp = Jsi_Strchr(option, '.');
        cb = Jsi_Strchr(option, '[');
    }
    if (cp && (!cb || cp<cb) ) {
        Jsi_DString dStr;
        int len = (cp-option);
        Jsi_DSInit(&dStr);
        cp = Jsi_DSAppendLen(&dStr, option, len);
        specPtr = Jsi_OptionsFind(interp, specs, cp, flags);
        Jsi_DSFree(&dStr);
        if (!specPtr || !specPtr->data|| specPtr->id != JSI_OPTION_CUSTOM || specPtr->custom != Jsi_Opt_SwitchSuboption) 
            return Jsi_LogError("unknown or bad sub-config option: %s", option);
        cp = option+len+1;
        return Jsi_OptionsSet(interp, (Jsi_OptionSpec *)(specPtr->data), (void*)(((char*)rec)+specPtr->offset), cp, valuePtr, flags);
    }
    if (cb && cb != option) {
        char *ce = Jsi_Strchr(option, ']');
        Jsi_Wide ul;
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        int len = 0;
        if (ce && ce>cb) {
            len = (ce-cb-1);
            cp = Jsi_DSAppendLen(&dStr, cb+1, len);
        }
        if (len <= 0 || Jsi_GetWide(interp, cp, &ul, 0) != JSI_OK || ul<0) 
            return Jsi_LogError("bad sub-array option: %s", option);
        len = (cb-option);
        Jsi_DSSetLength(&dStr, 0);
        cp = Jsi_DSAppendLen(&dStr, option, len);
        specPtr = Jsi_OptionsFind(interp, specs, cp, flags);
        Jsi_DSFree(&dStr);
        if (!specPtr || !specPtr->init.OPT_CARRAY|| specPtr->id != JSI_OPTION_CUSTOM || specPtr->custom != Jsi_Opt_SwitchCArray) {
bail:
            return Jsi_LogError("unknown or bad array option: %s", option);
        }
        cp = cb+1;
        Jsi_OptionSpec *subSpec = specPtr->init.OPT_CARRAY;
        int isize, size = specPtr->arrSize;
        if (!subSpec || size<=0 || (isize=subSpec->size)<=0)
            goto bail;
        isize = isize/size;
        uchar *s = (((uchar*)rec)+specPtr->offset + isize*ul);
        if (ce[1] != '.' || !subSpec->data) {
            if (Jsi_OptionsSet(interp, subSpec, (void*)s, subSpec->name, valuePtr, flags) != JSI_OK)
                return JSI_ERROR;
        } else {
            if (Jsi_OptionsSet(interp, (Jsi_OptionSpec *)subSpec->data, (void*)s, ce+2, valuePtr, flags) != JSI_OK)
                return JSI_ERROR;
        }
        return JSI_OK;
    }
    specPtr = Jsi_OptionsFind(interp, specs, option, flags);
    if (!specPtr)
        return JSI_ERROR;
    return jsi_SetOption(interp, specPtr, option, record, valuePtr, flags, isSafe);
}
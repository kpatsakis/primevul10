static Jsi_RC ValueToCArray(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    if (inStr)
        return JSI_ERROR;
    uchar *s = (uchar*)((char*)record) + spec->offset;
    Jsi_OptionSpec *subSpec = spec->init.OPT_CARRAY;
    int argc, i, isize, size = spec->arrSize;
    if (!subSpec || size<=0 || (isize=subSpec->size)<=0)
        goto bail;
    isize = isize/size;
    if (!Jsi_ValueIsArray(interp, inValue)) 
        return Jsi_LogError("expected array");
    argc = Jsi_ValueGetLength(interp, inValue);
    if (argc != size) 
        return Jsi_LogError("array length %d was not %d", argc, size);
    for (i = 0; i<size; i++) {
        Jsi_Value *v = Jsi_ValueArrayIndex(interp, inValue, i);
        if (Jsi_OptionsSet(interp, subSpec, (void*)s, subSpec->name, v, 0) != JSI_OK)
            return JSI_ERROR;
        s += isize;
    }
    return JSI_OK;
bail:
    return Jsi_LogError("bad config");
}
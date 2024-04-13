static Jsi_RC jsi_BitfieldToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue,
    Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    Jsi_csgset *bsget = spec->init.OPT_BITS;
    Jsi_Interp *d = interp;
    int idx = spec->idx;
    uchar *data = (uchar*)record;
    int64_t inum;
    Jsi_OptionSpec* enumSpec = (typeof(enumSpec))spec->data;

    if (!d || !bsget || idx<0) 
        return Jsi_LogBug("invalid bitfield");
    Jsi_RC rc = (*bsget)(interp, data, &inum, spec, idx, 0);
    if (rc != JSI_OK)
        return JSI_ERROR;

    if (enumSpec) {
        struct numStruct { int64_t numVal; } nval = { inum };
        Jsi_OptionSpec eSpec[] = {
            JSI_OPT(CUSTOM, struct numStruct, numVal, .help=spec->help, .flags=JSI_OPT_ENUM_SPEC, .custom=Jsi_Opt_SwitchEnum,
            .data=(void*)enumSpec, .info=0, .tname=spec->tname, .value=0, .bits=0, .boffset=8*sizeof(int64_t) ), //TODO: extra
            JSI_OPT_END(struct numStruct)
        };
        if (JSI_OK != jsi_EnumToValue(interp, eSpec, outValue, outStr, (void*)&nval, flags))
            return JSI_ERROR;
    } else if (outStr) {
        char obuf[JSI_MAX_NUMBER_STRING];
        snprintf(obuf, sizeof(obuf), "%" PRId64, inum);
        Jsi_DSAppend(outStr, obuf, NULL);
    } else {
        Jsi_Number num = (Jsi_Number)inum;
        Jsi_ValueMakeNumber(interp, outValue, num);
    }
    return JSI_OK;
}
static Jsi_RC jsi_ValueToBitfield(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue,
    const char *inStr, void *record, Jsi_Wide flags)
{
    Jsi_csgset *bsget = spec->init.OPT_BITS;
    Jsi_Interp *d = interp;
    int idx = spec->idx;
    uchar *data = (uchar*)record;
    Jsi_Number num;
    Jsi_OptionSpec* enumSpec = (typeof(enumSpec))spec->data;

    if (!d || !bsget || idx<0) 
        return Jsi_LogBug("invalid bitfield");
    if (enumSpec) {
        struct numStruct { int64_t numVal; } nval = {};
        Jsi_OptionSpec eSpec[] = {
            JSI_OPT(CUSTOM, typeof(nval), numVal, .help=spec->help, .flags=JSI_OPT_ENUM_SPEC,
                .custom=Jsi_Opt_SwitchEnum, .data=(void*)enumSpec, .info=0, .tname=spec->tname, .value=0, .bits=0, .boffset=8*sizeof(int64_t) ),
            JSI_OPT_END(typeof(nval))
        };
        if (JSI_OK != jsi_ValueToEnum(interp, eSpec, inValue, inStr, (void*)&nval, flags))
            return JSI_ERROR;
        num = (Jsi_Number)nval.numVal;
    } else if (inStr) {
        if (Jsi_GetDouble(interp, inStr, &num) != JSI_OK)
            return JSI_ERROR;
    } else {
        if (inValue && !Jsi_ValueIsNumber(interp, inValue)) 
            return JSI_ERROR;
        Jsi_ValueGetNumber(interp, inValue, &num);
    }
    int64_t inum = (int64_t)num;
    return (*bsget)(interp, data, &inum, spec, idx, 1);
}
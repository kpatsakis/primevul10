static Jsi_RC jsi_ValueToEnum(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    int n = 0;
    char *s = (char*)(((char*)record) + spec->offset);
    const char **list = (const char **)spec->data;
    Jsi_OptionSpec* es=NULL;
    int fflags = (flags|spec->flags);
    int cflags = (fflags&JSI_OPT_CUST_NOCASE?JSI_CMP_NOCASE:0);
    if (fflags&JSI_OPT_ENUM_EXACT)
        cflags |= JSI_CMP_EXACT;
    if (list && (fflags & JSI_OPT_ENUM_SPEC)) {
        es = (typeof(es))list;
        while (es->id != JSI_OPTION_END) es++;
        list = es->init.STRKEY;
        es = (typeof(es))spec->data;
    }
    if (!list) 
        return Jsi_LogError("custom enum spec did not set data: %s", spec->name);
    if (inStr) {
        if (JSI_OK != Jsi_GetIndex(interp, (char*)inStr, list, "enum", cflags, &n))
            return JSI_ERROR;
    } else
#ifndef JSI_LITE_ONLY
    if (JSI_OK != Jsi_ValueGetIndex(interp, inValue, list, "enum", cflags, &n))
        return JSI_ERROR;

#endif
    if (fflags&JSI_OPT_ENUM_UNSIGNED) {
        uint64_t u = (uint64_t)n;
        if (es)
            u = es[n].value;
        switch (spec->size) {
            case 1: *(uint8_t*)s = (uint8_t)u; break;
            case 2: *(uint16_t*)s = (uint16_t)u; break;
            case 4: *(uint32_t*)s = (uint32_t)u; break;
            case 8: *(uint64_t*)s = (uint64_t)u; break;
            default:
                return Jsi_LogError("enum size must be 1, 2, 4, or 8: %s", spec->name);
        }
    } else {
        int64_t m = n;
        if (es)
            m = es[n].value;
        switch (spec->size) {
            case 1: *(int8_t*)s = (int8_t)m; break;
            case 2: *(int16_t*)s = (int16_t)m; break;
            case 4: *(int32_t*)s = (int32_t)m; break;
            case 8: *(int64_t*)s = (int64_t)m; break;
            default: 
                return Jsi_LogError("enum size must be 1, 2, 4, or 8: %s", spec->name);
        }
    }
    
    return JSI_OK;
}
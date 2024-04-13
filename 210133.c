static Jsi_RC jsi_EnumToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    uint i = 0, j, esiz = 0;
    int64_t n;
    char *s = (char*)(((char*)record) + spec->offset);
    const char **list = (const char**)spec->data;
    Jsi_OptionSpec* es=NULL, *esp = NULL;
    int fflags = (flags|spec->flags);
    int uflag = (fflags&JSI_OPT_ENUM_UNSIGNED);
    if (list && (fflags & JSI_OPT_ENUM_SPEC)) {
        esp = es = (typeof(es))list;
        while (es->id != JSI_OPTION_END) es++;
        list = es->init.STRKEY;
        esiz = es->size;
        es = (typeof(es))spec->data;
    }
    if (!list) 
        return Jsi_LogError("custom enum spec did not set data: %s", spec->name);
    if (outStr) {
        if (uflag) {
            switch (spec->size) {
                case 1: n = *(uint8_t*)s; break;
                case 2: n = *(uint16_t*)s; break;
                case 4: n = *(uint32_t*)s; break;
                case 8: n = *(uint64_t*)s; break;
                default: 
                    return Jsi_LogError("enum size %d must be 1, 2, 4, or 8: %s", spec->size, spec->name);
            }
        } else {
            switch (spec->size) {
                case 1: n = *(int8_t*)s; break;
                case 2: n = *(int16_t*)s; break;
                case 4: n = *(int32_t*)s; break;
                case 8: n = *(int64_t*)s; break;
                default: 
                    return Jsi_LogError("enum size %d must be 1, 2, 4, or 8: %s", spec->size, spec->name);
            }
        }
        if (spec->flags&JSI_OPT_FMT_NUMBER) {
            Jsi_DSPrintf(outStr, "%" PRIu64, (uint64_t)n);
            return JSI_OK;
        }
        
        if (es) {
            for (j=0; j<esiz && list[j]; j++) {
                if (n == esp[j].value) {
                    i = j;
                    break;
                }
                if (j>=esiz)
                    i = esiz;
            }
        } else
            for (i=0; i<n && list[i]; i++) ; /* Look forward til n */
        if (list[i])
            Jsi_DSAppendLen(outStr, list[i], -1);
        else if ((spec->flags&JSI_OPT_COERCE)) {
            Jsi_DSPrintf(outStr, "%" PRIu64, (uint64_t)n);
            return JSI_OK;
        } else
            return Jsi_LogError("enum has unknown value: %d", *s);
        return JSI_OK;
    }
#ifndef JSI_LITE_ONLY
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_RC rc = jsi_EnumToValue(interp, spec, NULL, &dStr, record, flags);
    if (rc == JSI_OK)
        Jsi_ValueMakeStringKey(interp, outValue, Jsi_DSValue(&dStr));
    Jsi_DSFree(&dStr);
    return rc;
#endif
    return JSI_ERROR;
}
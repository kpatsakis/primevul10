static Jsi_RC jsi_BitsetToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    int i, n, cnt = 0, *s = (int*)(((char*)record) + spec->offset);
    const char **list = (const char**)spec->data;
    if (!list) 
        return Jsi_LogError("custom enum spec did not set data: %s", spec->name);
    if (outStr) {
        n = *s;
        for (i=0; list[i]; i++) {
            if (i >= (int)(spec->size*8)) 
                return Jsi_LogError("list larger than field size: %s", spec->name);
            if (!(n & (1<<i)))
                continue;
            if (cnt++)
                Jsi_DSAppendLen(outStr, ", ", 1);
            Jsi_DSAppendLen(outStr, list[i], -1);
        }
        return JSI_OK;
    }
#ifndef JSI_LITE_ONLY
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    n = *s;
    for (i=0; list[i]; i++) {
        Jsi_Value *v;
        if (!(n&(1<<i))) continue;
        cnt++;
        v = Jsi_ValueMakeStringKey(interp, NULL, list[i]);
        Jsi_ObjArrayAdd(interp, obj, v);
    }
    Jsi_ValueMakeArrayObject(interp, outValue, obj);
#endif
    return JSI_OK;
}
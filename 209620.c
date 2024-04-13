static Jsi_RC jsi_CArrayToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *dStr, void *record, Jsi_Wide flags)
{
    Jsi_Obj *obj = NULL;
    if (dStr)
        return JSI_ERROR;
    uchar *s = (uchar*)((char*)record) + spec->offset;
    Jsi_OptionSpec *subSpec = spec->init.OPT_CARRAY;
    int i, cnt, isize, size = spec->arrSize;
    if (!subSpec || size<=0 || (isize=subSpec->size)<=0)
        goto bail;
    isize = isize/size;
#ifndef JSI_LITE_ONLY //TODO: already in lite #ifdef
    obj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    cnt = 0;
    for (i=0; i<size; i++) {
        Jsi_Value *v = Jsi_ValueNew1(interp);
        cnt++;
        Jsi_RC rc = Jsi_OptionsGet(interp, subSpec, s, spec->name, &v, 0);
        Jsi_ObjArrayAdd(interp, obj, v);
        Jsi_DecrRefCount(interp, v);
        if (JSI_OK != rc)
            goto bail;
        s += isize;
    }
    Jsi_ValueMakeArrayObject(interp, outValue, obj);
#endif
    return JSI_OK;
bail:
    if (obj)
        Jsi_ObjFree(interp, obj);
    Jsi_LogError("bad config");
    return JSI_ERROR;
}
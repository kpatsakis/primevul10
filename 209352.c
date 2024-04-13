static Jsi_RC StringSplitCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this, //TODO: UTF
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    char **argv; int argc;
    int sLen, bLen, spLen = 1, noEmpty=0;
    Jsi_RC rc = JSI_OK;
    const char *v, *split = "";
    
    ChkString(_this, funcPtr, v, &sLen, &bLen);

    Jsi_Value *spliton = Jsi_ValueArrayIndex(interp, args, skip);
    
    if (spliton) {
        if (Jsi_ValueIsNull(interp, spliton))
            noEmpty = 1;
        else if (Jsi_ValueIsString(interp, spliton))
            split = Jsi_ValueString(interp, spliton, &spLen);
    }

    Jsi_DString sStr;
    Jsi_DSInit(&sStr);
    if (spLen)
        Jsi_SplitStr(v, &argc, &argv, split, &sStr);
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    int i, n = 0, siz = argc;
    if (noEmpty) {
        for (i=0, siz=0; i<argc; i++) if (argv[i][0]) siz++;
    } else if (!spLen)
        argc = siz = bLen;
    if (Jsi_ObjArraySizer(interp, obj, siz) <= 0) {
        rc = Jsi_LogError("index too large: %d", siz);
        Jsi_ObjFree(interp, obj);
        goto bail;
    }
    Jsi_ValueMakeArrayObject(interp, ret, obj);
    for (i = 0; i < argc; ++i) {
        if (noEmpty && !argv[i][0]) continue;
        char ctmp[2] = " ",  *cst = ctmp;
        if (!spLen)
            ctmp[0] = v[i];
        else
            cst = argv[i];
        Jsi_Value *v = Jsi_ValueNewStringDup(interp, cst);
        Jsi_IncrRefCount(interp, v);
        obj->arr[n++] = v;
    }
    Jsi_ObjSetLength(interp, obj, siz);
    
bail:
    Jsi_DSFree(&sStr);
    return rc;
}
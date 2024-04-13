Jsi_Value *Jsi_StringSplit(Jsi_Interp *interp, const char *str, const char *spliton)
{
    char **argv; int argc;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_SplitStr(str, &argc, &argv, spliton, &dStr);
    Jsi_Value *nret = Jsi_ValueNewArray(interp, NULL, 0);
    Jsi_Obj *obj = nret->d.obj;
    int i;
    for (i = 0; i < argc; ++i) {
        Jsi_ObjArraySet(interp, obj, Jsi_ValueNewStringDup(interp, argv[i]), i);
    }
    Jsi_ObjSetLength(interp, obj, argc);
    Jsi_ValueMakeArrayObject(interp, &nret, obj);
    Jsi_DSFree(&dStr);
    return nret;
}
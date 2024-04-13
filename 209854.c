Jsi_Value *jsi_LoadFunction(Jsi_Interp *interp, const char *str, Jsi_Value *tret) {
    Jsi_DString dStr = {};
    Jsi_Value *v;
    int i;
    const char *curFile = interp->curFile;
    interp->curFile = "<jsiLoadFunction>";
    for (i=0; i<2; i++) {
        Jsi_DSAppend(&dStr, "Jsi_Auto.", str, NULL);
        Jsi_VarLookup(interp, Jsi_DSValue(&dStr));
        v = Jsi_NameLookup(interp, Jsi_DSValue(&dStr));
        if (v)
            jsi_ValueDebugLabel(v, "jsiLoadFunction","f1");
        Jsi_DSFree(&dStr);
        if (v) {
            const char *cp = Jsi_ValueGetDString(interp, v, &dStr, 0);
            v = NULL;
            if (Jsi_EvalString(interp, cp, 0) == JSI_OK) {
                v = Jsi_NameLookup(interp, str);
                if (v)
                    jsi_ValueDebugLabel(v, "jsiLoadFunction","f2");
            }
            Jsi_DSFree(&dStr);
            if (v) {
                tret = v;
                break;
            }
        }
        if (interp->autoLoaded++ || i>0)
            break;
        /*  Index not in memory, so try loading Jsi_Auto from the autoload.jsi file. */
        if (interp->autoFiles == NULL)
            return tret;
        Jsi_Value **ifs = &interp->autoFiles;
        int i, ifn = 1;
        if (Jsi_ValueIsArray(interp, interp->autoFiles)) {
            ifs = interp->autoFiles->d.obj->arr;
            ifn = interp->autoFiles->d.obj->arrCnt;
        }
        for (i=0; i<ifn; i++) {  
            if (Jsi_EvalFile(interp, ifs[i], 0) != JSI_OK)
                break;
            interp->autoLoaded++;
        }
    }
    interp->curFile = curFile;
    return tret;
}
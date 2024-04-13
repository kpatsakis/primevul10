int Jsi_AddAutoFiles(Jsi_Interp *interp, const char *dir) {
    Jsi_DString dStr = {};
    Jsi_StatBuf stat;
    int i, cnt = 0;
    for (i=0; i<2; i++) {
        Jsi_DSAppend(&dStr, dir, (i==0?"/lib":""),"/autoload.jsi", NULL);
        Jsi_Value *v = Jsi_ValueNewStringKey(interp, Jsi_DSValue(&dStr));
        if (Jsi_Stat(interp, v, &stat) != 0)
            Jsi_ValueFree(interp, v);
        else {
            if (!interp->autoFiles) {
                interp->autoFiles = Jsi_ValueNewArray(interp, 0, 0);
                Jsi_IncrRefCount(interp, interp->autoFiles);
            }
            Jsi_ObjArrayAdd(interp, interp->autoFiles->d.obj, v);
            cnt++;
            interp->autoLoaded = 0;
        }
        Jsi_DSSetLength(&dStr, 0);
    }
    Jsi_DSFree(&dStr);
    return cnt;
}
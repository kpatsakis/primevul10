Jsi_RC Jsi_EvalZip(Jsi_Interp *interp, const char *exeFile, const char *mntDir, int *jsFound)
{
#if JSI__ZVFS==1
    Jsi_StatBuf stat;
    Jsi_Value *vinit, *linit, *vmnt = NULL;
    Jsi_Value *vexe = Jsi_ValueNewStringKey(interp, exeFile);
    Jsi_Value *ret = NULL;
    Jsi_RC rc;
    const char *omntDir = mntDir;
    int pass = 0;
    Jsi_IncrRefCount(interp, vexe);
    Jsi_HashSet(interp->genValueTbl, vexe, vexe);
    if (jsFound)
        *jsFound = 0;
    if (!mntDir)
        ret = Jsi_ValueNew(interp);
    else {
        vmnt = Jsi_ValueNewStringKey(interp, mntDir);
        Jsi_IncrRefCount(interp, vmnt);
        Jsi_HashSet(interp->genValueTbl, vmnt, vmnt);
    }
    bool osafe = interp->isSafe;
    if (interp->startSafe)
        interp->isSafe = 0;
    rc =Jsi_Mount(interp, vexe, vmnt, &ret);
    interp->isSafe = osafe;
    if (rc != JSI_OK)
        return rc;
    Jsi_DString dStr, bStr;
    Jsi_DSInit(&dStr);
    Jsi_DSInit(&bStr);
    if (!mntDir) {
        mntDir = Jsi_KeyAdd(interp, Jsi_ValueString(interp, ret, NULL));
        Jsi_DecrRefCount(interp, ret);
    }
dochk:
    Jsi_DSAppend(&dStr, mntDir, "/main.jsi", NULL);
    if (interp->pkgDirs)
        Jsi_ValueArrayPush(interp, interp->pkgDirs, Jsi_ValueNewStringKey(interp, mntDir));
    else {
        interp->pkgDirs = Jsi_ValueNewArray(interp, &mntDir, 1);
        Jsi_IncrRefCount(interp, interp->pkgDirs);
    }
    vinit = Jsi_ValueNewStringKey(interp,  Jsi_DSValue(&dStr));
    Jsi_IncrRefCount(interp, vinit);
    Jsi_HashSet(interp->genValueTbl, vinit, vinit);
    Jsi_DSFree(&dStr);
    Jsi_DSAppend(&dStr, mntDir, "/lib", NULL);
    const char *str = Jsi_DSValue(&dStr);
    linit = Jsi_ValueNewStringKey(interp, str);
    Jsi_IncrRefCount(interp, linit);
    if (Jsi_Stat(interp, linit, &stat) == 0)
        Jsi_ValueArrayPush(interp, interp->pkgDirs, linit);
    Jsi_DecrRefCount(interp, linit);
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&bStr);
    if (Jsi_Stat(interp, vinit, &stat) == 0) {
        if (jsFound)
            *jsFound |= JSI_ZIP_MAIN;
        interp->execZip = vexe;
        return Jsi_EvalFile(interp, vinit, JSI_EVAL_ARGV0|JSI_EVAL_AUTOINDEX);
    }
    if (Jsi_AddAutoFiles(interp, mntDir) && omntDir)
        *jsFound = JSI_ZIP_INDEX;
    if (!pass++) {
        str = Jsi_Strrchr(exeFile, '/');
        if (str) str++;
        else str = exeFile;
        char *bn = Jsi_DSAppend(&bStr, mntDir, "/", str, NULL);
        bn = Jsi_Strrchr(bn, '.');
        if (bn) *bn = 0;
        mntDir = Jsi_DSValue(&bStr);
        linit = Jsi_ValueNewStringKey(interp, mntDir);
        Jsi_IncrRefCount(interp, linit);
        int bsi = Jsi_Stat(interp, linit, &stat);
        Jsi_DecrRefCount(interp, linit);
        if (bsi == 0)
            goto dochk;
        Jsi_DSFree(&bStr);
    }
#endif
    return JSI_OK;
}
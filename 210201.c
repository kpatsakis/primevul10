char *Jsi_Realpath(Jsi_Interp *interp, Jsi_Value *src, char *newname)
{
    /* TODO: resolve pwd first. */
    void *data;
    const char *cp = NULL;
    Jsi_Filesystem *fsPtr;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    const char *npath = Jsi_ValueNormalPath(interp, src, &dStr);
    if (!npath) return NULL;
    Jsi_Value *tPtr = Jsi_ValueNew1(interp);
    Jsi_ValueMakeStringDup(interp, &tPtr, npath);
    fsPtr = Jsi_FilesystemForPath(interp, tPtr, &data);
    if (fsPtr) {
        if (fsPtr->realpathProc)
            cp = fsPtr->realpathProc(interp, src, newname);
        else if (!newname)
            cp = Jsi_Strdup(npath);
        else {
            Jsi_Strncpy(newname, npath, PATH_MAX);
            if (Jsi_Strlen(npath)>=PATH_MAX)
                newname[PATH_MAX-1] = 0;
        }
    }
    Jsi_DSFree(&dStr);
    Jsi_DecrRefCount(interp, tPtr);
    return (char*)cp;
}
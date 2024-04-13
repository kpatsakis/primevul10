int jsi_FSScandirProc(Jsi_Interp *interp, Jsi_Value *path, Jsi_Dirent ***namelist,
   int (*filter)(const Jsi_Dirent *), int (*compar)(const Jsi_Dirent **, const Jsi_Dirent**))
{
    const char *dirname = Jsi_ValueToString(interp, path, NULL);
    Jsi_DString dStr = {};
    if (*dirname == '~')
        dirname = jsi_TildePath(interp, dirname, &dStr);
    int rc = scandir(dirname, namelist, filter, compar);
    Jsi_DSFree(&dStr);
    return rc;
}
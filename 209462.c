static const char *jsi_TildePath(Jsi_Interp *interp, const char* path, Jsi_DString *dStr) {
    if (*path != '~')
        return path;
    const char *homedir = jsi_GetHomeDir(interp);
    if (!homedir)
        return path;
    Jsi_DSAppend(dStr, homedir, path[1] == '/' ? "" : "/", path+1, NULL);
    return Jsi_DSValue(dStr);
}
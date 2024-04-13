char *Jsi_FileRealpathStr(Jsi_Interp *interp, const char *path, char *newname)
{
    if (!path || !path[0]) return NULL;
    Jsi_DString dStr, eStr;
    char *npath = (char*)path, *apath;
    Jsi_DSInit(&dStr); Jsi_DSInit(&eStr);
    if (*path == '~') {
#ifndef __WIN32
        struct passwd pw, *pwp; /* TODO: could fallback to using env HOME. */
        char buf[JSI_BUFSIZ];
        int n = getpwuid_r(getuid(), &pw, buf, sizeof(buf), &pwp);
        const char *homedir = (n == 0 ? pwp->pw_dir : "");
        Jsi_DSAppend(&dStr, homedir, path[1] == '/' ? "" : "/", path+1, NULL);
#else
        const char *homedir = getenv("HOMEPATH");
        if (!homedir) homedir = "/";
        const char *homedrv = getenv("HOMEDRIVE");
        if (!homedrv) homedrv = "";
        Jsi_DSAppend(&dStr, homedrv, homedir, path[1] == '/' ? "" : "/", path+1, NULL);
#endif
        npath = Jsi_DSValue(&dStr);
    }
#ifdef __WIN32
    if (Jsi_Strncmp(path, JSI_ZVFS_DIR, sizeof(JSI_ZVFS_DIR)-1)==0 || Jsi_Strncmp(path, JSI_VFS_DIR, sizeof(JSI_VFS_DIR)-1)==0)
        apath = NULL;
    else
#endif
    apath = realpath(npath, newname);
    if (!apath) {
        if ((path[0] == '.' && path[1] == '/') || (path[0] != '/' && 
        !(path[0] == '.' && path[1] == '.') && path[1] != ':')) {
            Jsi_GetCwd(interp, &eStr);
            Jsi_DSAppend(&eStr, "/", path, NULL);
            npath = Jsi_DSValue(&eStr);
            apath = realpath(npath, newname);
            //npath = (char*)path;
        }
    }
    if (!apath) {
        if (newname)
            Jsi_Strcpy(apath=newname, npath);
        else
            apath = Jsi_Strdup(npath);
#ifndef __WIN32
        /* If path not exists on unix we try to eliminate ../ and /./ etc.*/
        NormalizeUnixPath(interp, apath);
#endif
    }
#ifdef __WIN32
    DeBackSlashify(apath);
#endif
    Jsi_DSFree(&dStr); Jsi_DSFree(&eStr);
    return apath;
}
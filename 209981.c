Jsi_Channel Jsi_Open(Jsi_Interp *interp, Jsi_Value *file, const char *modeString)
{
    /* Find fsys, and use open there. */
    Jsi_Filesystem *fsPtr;
    Jsi_Chan *ch = NULL;
    void *data;
    int fnl = 0;
    const char *fileName = Jsi_ValueString(interp, file, &fnl), *oldFN = fileName;
    if (!fileName || !fnl) {
        Jsi_LogError("expected string filename");
        return NULL;
    }
    if (!Jsi_Strcmp(fileName, "stdin")) return jsiIntData.stdChans;
    if (!Jsi_Strcmp(fileName, "stdout")) return jsiIntData.stdChans+1;
    if (!Jsi_Strcmp(fileName, "stderr")) return jsiIntData.stdChans+2;
    const char *s = modeString;
    bool quiet = 0;
    if (s[0]=='-') {
        quiet = true;
        s++;
    }
    char Mode[sizeof(ch->modes)];
    Jsi_StatBuf sb;
    Jsi_Value *path = NULL;
    int n, i, mode = 0, rc, writ, aflag;
    if (!s)
        s = "r";
    if (Jsi_Strlen(s) >= sizeof(ch->modes)) {
        Jsi_LogError("mode too long: %s", s);
        return NULL;
    }
    if (Jsi_Strchr(s, 'z') || Jsi_Strchr(s, 'Z')) {
        Jsi_Filesystem *fsPtr = jsi_FilesysFind("jfz");
        if (!fsPtr) {
            Jsi_LogError("compressed files unsupported");
            return NULL;
        }
        ch = fsPtr->openProc(interp, file, s);
        if (!ch) {
            if (!quiet)
                Jsi_LogError("File open failed '%s'", fileName);
            return NULL;
        }
        Jsi_Chan *nch = (Jsi_Chan *)Jsi_Calloc(1,sizeof(*nch));
        *nch = *ch;
        nch->fsPtr = fsPtr;
        return nch;
    }
    for (i=0, n = 0; s[i]; i++) {
        switch (s[i]) {
            case '+': break;
            case 'b': break;
            case 'r': if (!Jsi_Strchr(s,'+')) mode |= JSI_FS_READONLY; break;
            case 'a':
            case 'w': if (!Jsi_Strchr(s,'+')) mode |= JSI_FS_WRITEONLY; break;
            default: Jsi_LogError("unknown mode char: %c", s[i]); return NULL;
        }
        Mode[n++] = s[i];
    }
    Mode[n] = 0;
    /* Fixup files in the ~ dir */
    rc = Jsi_Stat(interp, file,&sb);
    if ((rc != 0 || *fileName == '~') && (fileName = Jsi_FileRealpath(interp, file, NULL))) {
        path = Jsi_ValueNewString(interp, fileName, -1);
        Jsi_IncrRefCount(interp, path);
        rc = Jsi_Stat(interp, path, &sb);
        if (rc == 0)
            file = path;
    }
    if (!fileName) {
        Jsi_LogError("file error: %s", oldFN);
        return NULL;
    }

    if (rc == 0 &&  sb.st_mode & S_IFDIR )
    {
        Jsi_LogError("can not open directory: %s", fileName);
        goto done;
    }
    fsPtr = Jsi_FilesystemForPath(interp, file, &data);
    writ = (Jsi_Strchr(s,'w') || Jsi_Strchr(s,'a') || Jsi_Strchr(s,'+'));
    aflag = (writ ? JSI_INTACCESS_WRITE : JSI_INTACCESS_READ);
    if (fsPtr && fsPtr != &jsiFilesystem) {
        ch = fsPtr->openProc(interp, file, Mode);
        if (ch)
            ch->isNative = 0;
        else
            Jsi_LogError("File open failed '%s'", fileName);
    } else {
        if (interp->isSafe && ((rc && Jsi_InterpAccess(interp, file, JSI_INTACCESS_CREATE) != JSI_OK)
        || Jsi_InterpAccess(interp, file, aflag) != JSI_OK)) {
            Jsi_LogError("%s access denied: %s", writ?"write":"read", fileName);
            goto done;
        }
        FILE *fp = fopen(fileName, Mode);
        fsPtr = &jsiFilesystem;
        if (!fp) {
            if (!quiet)
                Jsi_LogError("File open failed '%s'", fileName);
            goto done;
        }
        ch = (Jsi_Chan *)Jsi_Calloc(1,sizeof(*ch));
        ch->fp = fp;
        ch->isNative = 1;
    }
    if (ch) {
        ch->flags |= mode; // + (zLevel<<24);
        Jsi_Strcpy(ch->modes, s);
        ch->fsPtr = fsPtr;
        ch->fname = fileName;
    }
done:
    if (path)
        Jsi_DecrRefCount(interp, path);
    return ch;
}
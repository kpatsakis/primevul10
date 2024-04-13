Jsi_RC jsi_evalStrFile(Jsi_Interp* interp, Jsi_Value *path, const char *str, int flags, int level)
{
    Jsi_Channel tinput = NULL, input = Jsi_GetStdChannel(interp, 0);
    Jsi_Value *npath = path;
    Jsi_RC rc = JSI_ERROR;
    const char *ustr = NULL, *ostr = str;
    if (Jsi_MutexLock(interp, interp->Mutex) != JSI_OK)
        return rc;
    int oldSp, uskip = 0, fncOfs = 0, fnLen;
    int oldef = interp->evalFlags;
    jsi_Pstate *oldps = interp->ps;
    const char *oldFile = interp->curFile;
    char *origFile = Jsi_ValueString(interp, path, &fnLen);
    const char *fname = origFile;
    char *oldDir = interp->curDir, *cp;
    char dirBuf[PATH_MAX];
    jsi_Pstate *ps = NULL;
    jsi_FileInfo *fi = NULL;
    int exists = (flags&JSI_EVAL_EXISTS);
    int ignore = (flags&JSI_EVAL_ERRIGNORE);
    if (flags & JSI_EVAL_GLOBAL)
        level = 1;
    int oisi = interp->isMain;
    if (flags & JSI_EVAL_ISMAIN)
        interp->isMain = 1;
    
    oldSp = interp->framePtr->Sp;
    dirBuf[0] = 0;
    Jsi_DString dStr, fStr;
    Jsi_DSInit(&dStr);
    Jsi_DSInit(&fStr);

    if (str == NULL) {
        if (fname != NULL) {
            if (fnLen>2 && fname[fnLen-1]=='/') {
                Jsi_DSAppendLen(&fStr, fname, fnLen-1);
                const char *fcp = Jsi_DSValue(&fStr), *fce = strrchr(fcp,'/');
                if (fce) {
                    fname = Jsi_DSAppend(&fStr, fce, ".jsi", NULL);
                    npath = Jsi_ValueNewStringDup(interp, fname);
                    Jsi_IncrRefCount(interp, npath);
                }
            }
            if (!Jsi_Strcmp(fname,"-"))
                input = Jsi_GetStdChannel(interp, 0);
            else {
                /* Use translated FileName. */
                if (interp->curDir && fname[0] != '/' && fname[0] != '~') {
                    char dirBuf2[PATH_MAX], *np;
                    snprintf(dirBuf, sizeof(dirBuf), "%s/%s", interp->curDir, fname);
                    if ((np=Jsi_FileRealpathStr(interp, dirBuf, dirBuf2)) == NULL) {
                        Jsi_LogError("Can not open '%s'", fname);
                        goto bail;
                    }
                    if (npath != path)
                        Jsi_IncrRefCount(interp, npath);
                    npath = Jsi_ValueNewStringDup(interp, np);
                    Jsi_IncrRefCount(interp, npath);
                    fname = Jsi_ValueString(interp, npath, NULL);
                }
                if (flags&JSI_EVAL_ARGV0) {
                    if (interp->argv0)
                        Jsi_DecrRefCount(interp, interp->argv0);
                    interp->argv0 = Jsi_ValueNewStringDup(interp, fname);
                    Jsi_IncrRefCount(interp, interp->argv0);
                }
                
                bool osafe = interp->isSafe;
                if (interp->startSafe  && flags&JSI_EVAL_ARGV0) {
                    if (interp->safeReadDirs || interp->safeMode == jsi_safe_None)
                        interp->isSafe = 0;
                    else {
                        char vds[PATH_MAX], *cp;
                        const char *vda[2] = {vds};
                        Jsi_Strncpy(vds, Jsi_ValueString(interp, npath, NULL), sizeof(vds)-1);
                        vds[sizeof(vds)-1] = 0;
                        cp = Jsi_Strrchr(vds, '/');
                        if (cp)
                            cp[1] = 0;
                        Jsi_DString pStr = {};
                        vda[1] = Jsi_GetCwd(interp, &pStr);
                        interp->safeReadDirs = Jsi_ValueNewArray(interp, vda, 2);
                        Jsi_IncrRefCount(interp, interp->safeReadDirs);
                        if (interp->safeMode == jsi_safe_WriteRead || interp->safeMode == jsi_safe_Lockdown) {
                            if (!interp->safeWriteDirs) {
                                interp->safeWriteDirs = interp->safeReadDirs;
                                Jsi_IncrRefCount(interp, interp->safeWriteDirs);
                            }
                        } else if (interp->safeMode == jsi_safe_Write) {
                            interp->safeWriteDirs = Jsi_ValueNewArray(interp, vda+1, 1);
                            Jsi_IncrRefCount(interp, interp->safeWriteDirs);
                        }
                        Jsi_DSFree(&pStr);
                    }
                }
                tinput = input = Jsi_Open(interp, npath, (exists?"-r":"r"));
                interp->isSafe = osafe;
                if (!input) {
                    if (exists)
                        rc = JSI_OK;
                    //Jsi_LogError("Can not open '%s'", fname);
                    goto bail;
                }
            }
            cp = Jsi_Strrchr(fname, '.');
            if (cp && !Jsi_Strcmp(cp, ".jsi") && interp->isMain) {
                interp->typeCheck.parse = interp->typeCheck.run = interp->typeCheck.all = 1;
                interp->strict = 1;
            }
            bool isNew;
            Jsi_HashEntry *hPtr;
            hPtr = Jsi_HashEntryNew(interp->fileTbl, fname, &isNew);
            if (isNew == 0 && hPtr) {
                if ((flags & JSI_EVAL_ONCE)) {
                    rc = JSI_OK;
                    goto bail;
                }
                fi = (jsi_FileInfo *)Jsi_HashValueGet(hPtr);
                if (!fi) goto bail;
                interp->curFile = fi->fileName;
                interp->curDir = fi->dirName;
                
            } else {
                fi = (jsi_FileInfo *)Jsi_Calloc(1,sizeof(*fi));
                if (!fi) goto bail;
                Jsi_HashValueSet(hPtr, fi);
                fi->origFile = (char*)Jsi_KeyAdd(interp, origFile);
                interp->curFile = fi->fileName = (char*)Jsi_KeyAdd(interp, fname);
                char *dfname = Jsi_Strdup(fname);
                if ((cp = Jsi_Strrchr(dfname,'/')))
                    *cp = 0;
                interp->curDir = fi->dirName = (char*)Jsi_KeyAdd(interp, dfname);
                Jsi_Free(dfname);
            }
            if (!input->fname)
                input->fname = interp->curFile;

            int cnt = 0, noncmt = 0, jppOpts[4]={};
            uint ilen;
            char buf[JSI_BUFSIZ*2];
            const char *jpp;
            while (cnt<MAX_LOOP_COUNT) {
                if (!Jsi_Gets(interp, input, buf, sizeof(buf)))
                    break;
                if (++cnt==1 && (!(flags&JSI_EVAL_NOSKIPBANG)) && (buf[0] == '#' && buf[1] == '!')) {
                    Jsi_DSAppend(&dStr, "\n", NULL);
                    uskip=1;
                    continue;
                }
                if (!noncmt) {
                    int bi;
                    if (!buf[0] || (buf[0] == '/' && buf[1] == '/'))
                        goto cont;
                    for (bi=0; buf[bi]; bi++) if (!isspace(buf[bi])) break;
                    if (!buf[bi])
                        goto cont;
                }
                if (!noncmt++)
                    fncOfs = Jsi_DSLength(&dStr)-uskip;
                jpp = interp->jsppChars;
                if (jpp || interp->unitTest)
                    ilen = Jsi_Strlen(buf);
                if (interp->unitTest && buf[0]==';' && buf[1] && buf[2]) {
                    if (interp->unitTest&1 && jsi_JsPreprocessLine(interp, buf, sizeof(buf), ilen, jppOpts, cnt) != JSI_OK)
                        goto bail;
                } else if (interp->jsppCallback && interp->jsppChars) {
                    if (jsi_JsPreprocessLineCB(interp, buf, sizeof(buf), ilen, jppOpts, cnt) != JSI_OK)
                        goto bail;
                }
cont:
                Jsi_DSAppend(&dStr, buf,  NULL);
            }
            if (cnt>=MAX_LOOP_COUNT)
                Jsi_LogError("source file too large");
            str = Jsi_DSValue(&dStr);

        }
        if (interp->curDir && (flags&JSI_EVAL_AUTOINDEX))
            Jsi_AddAutoFiles(interp, interp->curDir);
    }
    ustr = str + fncOfs;
    // See if "use XXX" is on first non // or empty line (or second if there is a #! on first line)
    if (ustr && *ustr && !Jsi_Strncmp(ustr+uskip, "\"use ", 5)) {
        ustr += 5+uskip;
        const char *cpe = ustr;
        while (*cpe && *cpe != '\"' && *cpe != '\n' && (isalpha(*cpe) || *cpe ==',' || *cpe =='!')) cpe++;
        if (*cpe == '\"') {
            Jsi_DString cStr;
            Jsi_DSInit(&cStr);
            cpe = Jsi_DSAppendLen(&cStr, ustr, (cpe-ustr));
            rc = jsi_ParseTypeCheckStr(interp, cpe);
            Jsi_DSFree(&cStr);
            if (rc != JSI_OK)
                goto bail;
        }
    }

    /* TODO: cleanup interp->framePtr->Sp stuff. */
    oldSp = interp->framePtr->Sp;
    // Evaluate code.
    rc = JSI_OK;
    ps = jsiNewParser(interp, str, input, 0);
    interp->evalFlags = flags;
    if (!ps)
        rc = JSI_ERROR;
    else {
        Jsi_ValueMakeUndef(interp, &interp->retValue);
        interp->ps = ps;
        Jsi_Value *retValue = interp->retValue;
        if (!interp->strict)
            interp->strict = (jsi_GetDirective(interp, ps->opcodes, "use strict")!=NULL);
        const char *curFile = interp->curFile;

        if (level <= 0)
            rc = jsi_evalcode(ps, NULL, ps->opcodes, interp->gsc, interp->csc, interp->csc, &retValue);
        else {
            jsi_Frame *fptr = interp->framePtr;
            while (fptr && fptr->level > level)
                fptr = fptr->parent;
            if (!fptr)
                rc = JSI_ERROR;
            else
                rc = jsi_evalcode(ps, NULL, ps->opcodes, fptr->ingsc, fptr->incsc, fptr->inthis, &retValue);
        }
        interp->curFile = curFile;
        if (rc != JSI_OK)
            rc = (ignore?JSI_OK:JSI_ERROR);
        else if (ps->last_exception || oldps->last_exception)
            Jsi_ValueDup2(interp, &oldps->last_exception, ps->last_exception); //TODO: dup even if null?
        interp->ps = oldps;
        interp->evalFlags = oldef;
        if (rc == JSI_OK && ps && !ostr)
            fi->str = ps->lexer->d.str;
    }
    
bail:
    interp->curFile = oldFile;
    interp->curDir = oldDir;
    interp->framePtr->Sp = oldSp;
    interp->isMain = oisi;
    if (path != npath)
        Jsi_DecrRefCount(interp, npath);
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&fStr);
    if (tinput)
        Jsi_Close(interp, tinput);
    Jsi_MutexUnlock(interp, interp->Mutex);
    if (interp->exited && interp->level <= 0)
    {
        rc = JSI_EXIT;
        if (!interp->parent)
            Jsi_InterpDelete(interp);
    }

    return rc;
}
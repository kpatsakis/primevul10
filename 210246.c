char* Jsi_NormalPath(Jsi_Interp *interp, const char *path, Jsi_DString *dStr) {
    char prefix[3] = "";
    char cdbuf[PATH_MAX];
    Jsi_DSInit(dStr);
    if (!path || !path[0]) return NULL;
    if (*path == '/')
        Jsi_DSAppend(dStr, path, NULL);
#ifdef __WIN32  /* TODO: add proper handling for windows paths. */
    else if (*path && path[1] == ':') {
        Jsi_DSAppend(dStr, path, NULL);
        return Jsi_DSValue(dStr);
    }
#endif
    else if (path[0] == '~') {
        Jsi_DSAppend(dStr, jsi_GetHomeDir(interp), (path[1] == '/' ? "" : "/"), path+1, NULL);
    } else if (path[0] == '.' && path[1] == 0) {
        if (jsiIntData.pwd) {
            Jsi_DSAppend(dStr, jsiIntData.pwd, NULL);
        } else {
            Jsi_DSAppend(dStr, getcwd(cdbuf, sizeof(cdbuf)), NULL);
        }
    } else {
        if (jsiIntData.pwd) {
            Jsi_DSAppend(dStr, jsiIntData.pwd, "/", path, NULL);
        } else {
            Jsi_DSAppend(dStr, getcwd(cdbuf, sizeof(cdbuf)), "/", path, NULL);
        }
    }
    Jsi_DString sStr = {};
    char *cp = Jsi_DSValue(dStr);
#ifdef __WIN32
    if (*cp && cp[1] == ':') {
        prefix[0] = *cp;
        prefix[1] = cp[1];
        prefix[2] = 0;
        cp += 2;
    }
#endif
    int i=0, n=0, m, nmax, unclean=0, slens[PATH_MAX];
    char *sp = cp, *ss;
    char *sptrs[PATH_MAX];
    while (*cp && n<PATH_MAX) {
        while (*cp && *cp == '/') {
            cp++;
            if (*cp == '/')
                unclean = 1;
        }
        sptrs[n] = cp;
        if (cp[0] == '.' && (cp[1] == '.' || cp[1] == '/'))
            unclean = 1;
        ss = cp++;
        while (*ss && *ss != '/')
            ss++;
        slens[n++] = (ss-cp) + 1;
        cp = ss;
    }
    if (!unclean)
        return sp;
    /* Need to remove //, /./, /../ */
    nmax = n--;
    while (n>0) {
        if (slens[n]<=0) {
            n--;
            continue;
        }
        if (Jsi_Strncmp(sptrs[n],".",slens[n])==0)
            slens[n] = 0;
        else if (Jsi_Strncmp(ss,"..",slens[n])==0) {
            int cnt = 0;
            m = n-1;
            while (m>=0 && cnt<1) {
                if (slens[m])
                    cnt++;
                slens[m] = 0;
                m--;
            }
            if (cnt<1)
                return sp;  /* Can't fix it */
        }
        n--;
    }
    /* TODO: prefix for windows. */
    Jsi_DSAppend(&sStr, prefix, NULL);
    for (i=0; i<nmax; i++) {
        if (slens[i]) {
#ifdef __WIN32
            Jsi_DSAppend(&sStr, "/" /*"\\"*/, NULL);
#else
            Jsi_DSAppend(&sStr, "/", NULL);
#endif
            Jsi_DSAppendLen(&sStr, sptrs[i], slens[i]);
        }
    }
    Jsi_DSSetLength(dStr, 0);
    Jsi_DSAppend(dStr, Jsi_DSValue(&sStr), NULL);
    Jsi_DSFree(&sStr);
    return Jsi_DSValue(dStr);
}
static void NormalizeUnixPath(Jsi_Interp *interp, char *path) {
    char **argv; int argc, i;
    if (!Jsi_Strstr(path, "./")) return;
    Jsi_DString dStr = {}, eStr = {};
    if (path[0] != '/' && Jsi_Strstr(path, "..")) {
        char *npath = Jsi_GetCwd(interp, &eStr);
        if (npath && Jsi_Strcmp(npath,"/")) {
            Jsi_DSAppend(&eStr, "/", path, NULL);
            path = Jsi_DSValue(&eStr);
        }
    }
    Jsi_DString sStr;
    Jsi_DSInit(&sStr);
    Jsi_SplitStr(path, &argc, &argv, "/", &sStr);
    char *cp = path;
    *cp = 0;
    for (i=0; i<argc; i++) {
        if (i == 0 && argv[0][0] == 0) {
            continue;
        } else if (argv[i][0] == 0) {
            continue;
        } else if (i && !Jsi_Strcmp(argv[i],".")) {
            continue;
        } else if (!Jsi_Strcmp(argv[i],"..")) {
            char *pcp = Jsi_DSValue(&dStr), *lcp = pcp;
            pcp = Jsi_Strrchr(pcp, '/');
            if (pcp && pcp != Jsi_DSValue(&dStr)) {
                *pcp = 0;
                Jsi_DSSetLength(&dStr, Jsi_Strlen(lcp));
            }
            continue;
        } else {
            Jsi_DSAppend(&dStr, (i>0?"/":""), argv[i], NULL);
        }
    }
    Jsi_DSFree(&sStr);
    Jsi_Strcpy(path, Jsi_DSValue(&dStr));
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&eStr);
}
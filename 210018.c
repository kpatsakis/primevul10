static void jsi_wsPathAlias(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, const char **inPtr, Jsi_DString *dStr) {
    const char *ce, *cp = NULL;
    char *lcp;
    Jsi_Value *val;
    if (cmdPtr->pathAliases) {
        cp = *inPtr;
        if (*cp == '/') cp++;
        if ((ce = Jsi_Strchr(cp, '/'))) {
            int len = ce-cp;
            Jsi_DSSetLength(dStr, 0);
            Jsi_DSAppendLen(dStr, cp, len);
            cp = Jsi_DSValue(dStr);
            if ((val = Jsi_ValueObjLookup(interp, cmdPtr->pathAliases, cp, 0)) &&
                (cp = Jsi_ValueString(interp, val, NULL))) {
                *inPtr += (len+2);
                cmdPtr->curRoot = cp;
                return;
            }
        }
    }
    if (!Jsi_Strncmp(*inPtr, "/jsi/", 5)) {
        // Get/cache path for system load file, eg /zvfs/lib/Jsish.jsi.
        if (!(cp = cmdPtr->jsishPathCache)) {
            Jsi_PkgRequire(interp, "Jsish", 0);
            if (Jsi_PkgVersion(interp, "Jsish", &cp)>=0)
                cmdPtr->jsishPathCache = cp;
        }
        if (cp) {
            Jsi_DSSetLength(dStr, 0);
            Jsi_DSAppend(dStr, cp, NULL);
            cp = Jsi_DSValue(dStr);
            if ((lcp = Jsi_Strrchr(cp, '/'))) {
                *lcp = 0;
                *inPtr += 5;
                cmdPtr->curRoot = cp;
            }
        }
    }
}
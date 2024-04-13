static Jsi_RC MySqlExtractParmNames(MySqlObj* jdb, const char *sql, Jsi_DString *sStr, Jsi_DString *nStr) {
    const char *ocp, *cp = sql;
    int cnt = 0;
    while (*cp) {
        if (*cp == '\"'||*cp == '\'') {
            ocp = cp;
            cp = mdbFindEndStr(cp);
            if (!cp)
                return JSI_ERROR;
            Jsi_DSAppendLen(sStr, ocp, cp-ocp+1);
        } else if ((*cp == '@' || *cp == ':' || *cp == '$' ) && (isalpha(cp[1]) || cp[1] == '_')) {
            ocp = cp;
            cp+=2;
            while (*cp && (isalnum(*cp) || *cp == '_'))
                cp++;
            if (*ocp == '$' && *cp == '(') {
                const char *ttp = NULL, *ttb = NULL, *eq = NULL;
                cp++;
                if (*cp == '[')
                    eq = cp++;
                while (*cp && (isalnum(*cp) || *cp == '_' || *cp == ':' || *cp == '.' || *cp == ']')) {
                    if (*cp == ':') {
                        if (ttp)
                            return JSI_ERROR;
                        ttp = cp;
                    }
                    if (*cp == ']') {
                        if (ttb)
                            return JSI_ERROR;
                        ttb = cp;
                        if (cp[1] != ')' && cp[1] != ':' && cp[1] != '.')
                            return JSI_ERROR;
                    }
                    cp++;
                }
                if (*cp != ')')
                    return JSI_ERROR;
                if (eq && !ttb)
                    return JSI_ERROR;
                if (ttp) {
                    Jsi_DString tStr = {};
                    Jsi_DSAppendLen(&tStr, ttp+1, (cp - ttp - 1));
                    if (!jdb->typeNameHash) mdbTypeNameHashInit(jdb);
                    int rc = (Jsi_HashEntryFind(jdb->typeNameHash, Jsi_DSValue(&tStr)) != NULL);
                    if (!rc) {
                        Jsi_DString eStr = {};
                        Jsi_HashEntry *hPtr;
                        Jsi_HashSearch search;
                        Jsi_Interp *interp = jdb->interp;
                        int n = 0;
                        for (hPtr = Jsi_HashSearchFirst(jdb->typeNameHash, &search);
                            hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
                            const char *key = (char*)Jsi_HashKeyGet(hPtr);
                            Jsi_DSAppend(&eStr, (n++?", ":""), key, NULL);
                        }
                        Jsi_LogWarn("bind type \"%s\" is not one of: %s", Jsi_DSValue(&tStr), Jsi_DSValue(&eStr));
                        Jsi_DSFree(&eStr);
                    }
                    Jsi_DSFree(&tStr);
                    if (!rc)
                        return JSI_ERROR;
                }
            } else
                cp--;
            if (cnt++)
                Jsi_DSAppendLen(nStr, " ", 1);
            Jsi_DSAppendLen(nStr, ocp, cp-ocp+1);
            Jsi_DSAppendLen(sStr, "?", 1);
        } else if (*cp == '\\' && cp[1]) {
            Jsi_DSAppendLen(sStr, cp, 2);
            cp++;
        } else {
            Jsi_DSAppendLen(sStr, cp, 1);
        }
        cp++;
    }
    return JSI_OK;
}
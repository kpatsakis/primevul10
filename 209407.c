static Jsi_RC jsi_wsEvalSSI(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, Jsi_Value *fn, Jsi_DString *dStr,
    int lvl, jsi_wsPss *pss) {
    int flen, rlen;
    char fbuf[PATH_MAX];
    char *fs, *fname = Jsi_ValueString(interp, fn, &flen), *fend = fname;
    if (lvl>10 || !fname || !pss)
        return JSI_ERROR;
    Jsi_Value *fval;
    Jsi_RC rc = JSI_OK;
    Jsi_DString tStr = {}, lStr = {};
    const char *cs = NULL;
    char *root = Jsi_ValueString(interp, cmdPtr->rootdir, &rlen);
    if (!Jsi_Strncmp(root, fname, rlen))
        fname = fname + rlen;
    fs = Jsi_Strrchr(fname, '/');
    if (fs) {
        flen = fs-fname;
        fend = fs+1;
    }
    if (lvl>0) {
        rc = jsi_wsFileRead(interp, fn, &tStr, cmdPtr, pss);
        cs = Jsi_DSValue(&tStr);
    } else {
        snprintf(fbuf, sizeof(fbuf), "%s/%.*s/%s", root, flen, fname, cmdPtr->includeFile);
        fval = Jsi_ValueNewStringConst(interp, fbuf, -1);
        Jsi_IncrRefCount(interp, fval);
        rc = jsi_wsFileRead(interp, fval, &tStr, cmdPtr, pss);
        if (rc == JSI_OK)
            cs = Jsi_DSValue(&tStr);
        Jsi_DecrRefCount(interp, fval);
    }
    
    char *cp, *sp, *se, pref[] = "<!--#", suffix[] = "-->", *msg = NULL;
    struct {
        int inif, inelse, matched, elide;
    } II[11] = {};
    const int maxNest = 10;
    int ii = 0;
    int plen = 5, elen, llen;

    while (rc == JSI_OK && cs) {
        char *ext = NULL, *sfname = fname;
        int sflen = flen;
        sp = Jsi_Strstr(cs, pref);
        if (!sp || !(se=Jsi_Strstr(sp+plen, suffix))) {
            Jsi_DSAppend(dStr, cs, NULL);
            break;
        }
        sp += plen-1;
        llen = se-sp;
        Jsi_DSSetLength(&lStr, 0);
        cp = Jsi_DSAppendLen(&lStr, sp, llen);
        if (Jsi_Strchr(cp, '\n')) { rc = Jsi_LogError("unexpected newline in directive \"%.10s\"", cp); break; }
        if (!II[ii].elide)
            Jsi_DSAppendLen(dStr, cs, sp-cs-4);
        
        if (!Jsi_Strncmp(cp, "#include file=\"", 12) || !Jsi_Strncmp(cp, "#include virtual=\"", 15)) {
            if (cp[llen-1] != '"' || cp[llen-2] == '=') { msg = "missing end quote in"; break; }
            if (!II[ii].elide) {
                Jsi_DSSetLength(&lStr, llen-1);
                int isvirt = (cp[9]=='v');
                cp += (isvirt ? 18 : 15);
                if (cp[0] == '$' && lvl == 0) { // substitute file suffix
                    char sfx[20] = {};
                    uint i;
                    for (i=0; i<sizeof(sfx); i++) {
                        if ((sfx[i] = cp[i+1]) == '"' || !sfx[i]) {
                            sfx[i] = 0;
                            break;
                        }
                    }
                    snprintf(fbuf, sizeof(fbuf), "%s/%.*s/%s/%s%s", root, flen, fname, sfx, fend, sfx);
                } else {
                    snprintf(fbuf, sizeof(fbuf), "%s/%.*s/%s", root, sflen, sfname, cp);
                    ext = Jsi_Strrchr(fbuf, '.');
                }
                fval = Jsi_ValueNewStringConst(interp, fbuf, -1);
                Jsi_IncrRefCount(interp, fval);
                if (!ext || ext[0] != '.' || !jsi_wsIsSSIExt(interp, cmdPtr, pss, ext+1))
                    rc = jsi_wsFileRead(interp, fval, dStr, cmdPtr, pss);
                else
                    rc = jsi_wsEvalSSI(interp, cmdPtr, fval, dStr, lvl+1, pss);
                if (cmdPtr->noWarn)
                    rc = JSI_OK;
                Jsi_DecrRefCount(interp, fval);
            }
        } else if (!Jsi_Strncmp(cp, "#echo \"${", 9)) {
            if (cp[llen-1] != '"' || cp[llen-2] != '}') { msg = "missing end quote"; break; }
            Jsi_DSSetLength(&lStr, llen-2);
            cp += 9;
            llen -= 9;
            if (!Jsi_Strcmp(cp, "#")) {
                if (!pss->key[0])
                    snprintf(pss->key, sizeof(pss->key), "%d%p%d", pss->wid, pss, (int)cmdPtr->startTime);
                Jsi_DSPrintf(dStr, "'%s'", pss->key);
            } else {
                Jsi_Value *val = NULL;
                if (!cmdPtr->udata) {
                    val = Jsi_ValueObjLookup(interp, cmdPtr->udata, cp, 0);
                    if (!val) { msg = "udata lookup failure"; break; }
                    cp = Jsi_ValueString(interp, val, NULL);
                    Jsi_DSPrintf(dStr, "'%s'", cp);
                }
            }

        } else if (!Jsi_Strncmp(cp, "#if expr=\"", 10) || !Jsi_Strncmp(cp, "#elif expr=\"", 12)) {
            if (llen<11 || cp[llen-1] != '"' || cp[llen-2] == '=') { msg = "missing end quote"; break; }
            Jsi_DSSetLength(&lStr, llen-1);
            bool iselif = (cp[1]=='e');
            cp += (iselif?12:10);
            if (!iselif) {
                if (II[ii].inif) {
                    if ((ii+1)>=maxNest) { msg = "nested \"#if\" too deep"; break; }
                    ii++;
                    II[ii] = II[maxNest];
                    II[ii].elide = II[ii-1].elide;
                }
                II[ii].inif = 1;
            } else {
                if (!II[ii].inif) { msg = "unexpected \"#elif\""; break; }
            }
            elen = Jsi_Strlen(cp);
            if (elen<4|| cp[0] != '$' || cp[1] != '{' || cp[elen-1] != '}') {
                msg = "expr must be of form ${X}"; break;
            }
            Jsi_DSSetLength(&lStr, llen-2);
            cp += 2;
            // Extract modifiers before bool var name to lookup.
            bool warn = 0, req = 0, nifval = 0, not = 0, isq=0, isu=0, qfirst=0;
            while (*cp &&  !isalpha(*cp)) {
                bool fail = 0;
                switch (*cp) {
                    case '~': qfirst = 1; break;
                    case ':': isu = 1; break;
                    case '?': isq = 1; break;
                    case '@': warn = !II[ii].matched; break;
                    case '*': req = !II[ii].matched; break;
                    case '!': not = 1; break;
                    default: fail=1; break;
                }
                if (fail) { msg = "modifier must be one of: !:=?@*"; break; }
                cp++;
            }
            Jsi_Value *val = NULL;
            if (!val && qfirst && pss->queryObj)
                val = Jsi_ValueObjLookup(interp, pss->queryObj, cp, 0);
            if (!val && !isq && cmdPtr->udata)
                val = Jsi_ValueObjLookup(interp, cmdPtr->udata, cp, 0);
            if (!val && !qfirst && !isu && pss->queryObj)
                val = Jsi_ValueObjLookup(interp, pss->queryObj, cp, 0);
            if (!val) {
                if (req) { msg = "symbol not found"; break; }
                if (warn) Jsi_LogWarn("symbol \"%s\" not found: %s", cp, fbuf);
            } else if (Jsi_ValueGetBoolean(interp, val, &nifval) != JSI_OK) {
                const char *valStr = NULL;
                if (val) valStr = Jsi_ValueString(interp, val, NULL);
                if (!valStr || Jsi_GetBool(interp, valStr, &nifval) != JSI_OK) {
                    if (!warn) { msg = "symbol not a boolean"; break; }
                    Jsi_LogWarn("symbol \"%s\" should be a boolean: %s", cp, fbuf);
                }
            }
            if (not) nifval = !nifval;
            if (!iselif) {
                if (nifval)
                    II[ii].matched = 1;
                else
                    II[ii].elide = 1;
            } else {
                if (II[ii].matched || !nifval)
                    II[ii].elide = 1;
                else if (nifval) {
                    II[ii].matched = 1;
                    II[ii].elide = (ii?II[ii-1].elide:0);
                }
            }
        } else if (!Jsi_Strncmp(cp, "#else", 5)) {
            if (!II[ii].inif || II[ii].inelse) { msg = "unexpected \"#else\""; break; }
            II[ii].inelse = 1;
            II[ii].elide = (ii&&II[ii-1].elide?1:II[ii].matched);
        } else if (!Jsi_Strncmp(cp, "#endif", 6)) {
            if (!II[ii].inif) { msg = "unexpected \"#endif\"";  break; }
            II[ii].inelse = II[ii].inif = II[ii].elide = II[ii].matched = 0;
            if (ii>0)
                ii--;
        } else {
            msg = "expected directive #include/#if/#elif/#else/#endif";
            break;
        }
        cs = se + 3;
        if (*cs == '\n')
            cs++;
    }
    if (rc == JSI_OK && II[ii].inif && !msg) {
         msg = "missing \"#endif\"";
         sp = "";
    }
    if (msg) {
        while (*fname=='/') fname++;
        rc = Jsi_LogError("SHTML Error in \"%s\": %s: at \"%.40s\" ", fname, msg, sp);
    }
    Jsi_DSFree(&tStr);
    Jsi_DSFree(&lStr);
    return rc;

}
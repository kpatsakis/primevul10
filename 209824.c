Jsi_Regex* Jsi_RegExpNew(Jsi_Interp *interp, const char *regtxt, int eflag)
{
    bool isNew;
    Jsi_HashEntry *hPtr;
    int flag = REG_EXTENDED;
    char c, *cm, *ce;
    const char *cp;
    Jsi_Regex *re;
    if (interp->subOpts.noRegex) {
        Jsi_LogError("regex disabled for interp");
        return NULL;
    }

    eflag |= JSI_REG_STATIC;
    if (!regtxt[0])
        return NULL;
    hPtr = Jsi_HashEntryFind(interp->regexpTbl, regtxt);
    if (hPtr) {
        re = (Jsi_Regex*)Jsi_HashValueGet(hPtr);
        if (JSI_REG_STATIC & eflag)
            re->eflags |= JSI_REG_STATIC;
        return re;
    }
    cp = regtxt+1;
    if (regtxt[0] != '/')
        return NULL;
    ce = (char*)Jsi_Strrchr(cp, '/');
    if (ce == cp || !ce)
        return NULL;
    cm = ce + 1;    
    while (*cm) {
        c = *cm++;
        if (c == 'i') flag |= REG_ICASE;
        else if (c == 'g') eflag |= JSI_REG_GLOB;
        else if (c == 'm') { /* PERL NON-STANDARD */
            eflag |= JSI_REG_NEWLINE;
            flag |= REG_NEWLINE;
        }
#ifdef RE_DOT_NEWLINE
        else if (c == 's') { /* PERL NON-STANDARD */
            eflag |= JSI_REG_DOT_NEWLINE;
            flag |= RE_DOT_NEWLINE;
        }
#endif
    }
    *ce = 0;
    regex_t reg;
    if (regcomp(&reg, cp, flag)) {
        *ce++ = '/';
        Jsi_LogError("Invalid regex string '%s'", cp);
        return NULL;
    }
    *ce++ = '/';
    re = (Jsi_Regex*)Jsi_Calloc(1, sizeof(Jsi_Regex));
    SIGINIT(re, REGEXP);
    assert (re);
    re->reg = reg;
    re->eflags = eflag;
    re->flags = flag;
    hPtr = Jsi_HashEntryNew(interp->regexpTbl, regtxt, &isNew);
    assert(hPtr);
    Jsi_HashValueSet(hPtr, re);
    re->pattern = (char*)Jsi_HashKeyGet(hPtr);
    return re;

}
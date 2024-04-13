const char* jsi_FuncGetCode(Jsi_Interp *interp, Jsi_Func *func, int *lenPtr) {
    if (interp->subOpts.noFuncString || !func->bodyStr)
        return NULL;
    const char *cp, *cp2;
    if (func->startPos == -1) {
        cp = func->bodyStr;
        int cplin = func->bodyline.last_line-1;
        while (*cp && cplin>0) {
            if (*cp=='\n' && --cplin<=0)
                break;
            cp++;
        }
        while (*cp && isspace(*cp))
            cp++;
        func->startPos = (*cp?(cp - func->bodyStr):-2);
    }
    if (func->startPos >= 0) {
        int len = func->endPos - func->startPos;
        cp = func->bodyStr + func->startPos;
        while (len>0 && (isspace(cp[len-1]) || cp[len-1]==';')) len--;
        if (*cp != 'f' && Jsi_Strncmp(cp, "function", 8) && (cp2=Jsi_Strstr(cp, "function"))) {
            len -= (cp2-cp);
            cp = cp2;
        }
        *lenPtr = len;
        return cp;
    }
    return NULL;
}
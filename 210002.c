void Jsi_SplitStr(const char *str, int *argcPtr,
              char ***argvPtr, const char *ch, Jsi_DString *dStr)
{
    char *cp, *ep, *p, **argv;
    int cnt = 1, len, i, clen;
    if (!ch)
        ch = "";
    clen = Jsi_Strlen(ch);
    if (clen<=0)
        return SplitChar(str, argcPtr, argvPtr, *ch, dStr);
    len = Jsi_Strlen(str);
    cp = (char*)str;
    while (*cp) {
        cp = Jsi_Strstr(cp,ch);
 
        if (cp == NULL || *cp == 0) break;
        cp += clen;
        cnt++;
    }
    //argv = (char**)Jsi_Calloc(1,(sizeof(char*)*(cnt+3) + sizeof(char)*(len+6)));
    Jsi_DSSetLength(dStr, (sizeof(char*)*(cnt+3) + sizeof(char)*(len+6)));
    argv = (char**)Jsi_DSValue(dStr);
    *argvPtr = argv;
    *argcPtr = cnt;
    p = (char*)&(argv[cnt+2]);
    argv[cnt+1] = p;
    Jsi_Strcpy(p, str);
    cp = p;
    i = 0;
    argv[i++] = p;
    while (*cp) {
        ep = Jsi_Strstr(cp,ch);
        if (ep == NULL || *ep == 0) break;
        *ep = 0;
        cp = ep+clen;
        argv[i++] = cp;
    }
    argv[cnt] = NULL;
}
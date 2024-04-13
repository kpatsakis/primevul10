static void SplitChar(const char *str, int *argcPtr,
              char ***argvPtr, char ch, Jsi_DString *dStr)
{
    char *cp, *ep, *p, **argv;
    int cnt = 1, len, i;

    len = Jsi_Strlen(str);
    cp = (char*)str;
    while (*cp) {
        if (ch)
            cp = Jsi_Strchr(cp,ch);
        else {
            while (*cp && !isspace(*cp))
                cp++;
        }
        if (cp == NULL || *cp == 0) break;
        cp++;
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
        if (ch)
            ep = Jsi_Strchr(cp,ch);
        else {
            ep = cp;
            while (*ep && !isspace(*ep))
                ep++;
        }
        if (ep == NULL || *ep == 0) break;
        *ep = 0;
        cp = ep+1;
        argv[i++] = cp;
    }
    argv[cnt] = NULL;
}
char *jsiLNhints(const char *buf, int *color, int *bold) {
    int i, len = Jsi_Strlen(buf);
    for (i=0; jsiFilePreCmds[i]; i++)
        if (!Jsi_Strncmp(buf, jsiFilePreCmds[i], Jsi_Strlen(jsiFilePreCmds[i]))) break;
    if (jsiFilePreCmds[i]) {
        const char *ce = buf+len-1, *cp = "('<file>";
        if ((*ce =='\'' || *ce =='\"') && buf[len-2]=='(') cp+=2;
        else if (*ce=='(') cp++;
        else return NULL;
        
        *color = 35;
        *bold = 0;
        return (char*)cp;
    }
    return NULL;
}
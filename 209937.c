static char *jsiRlCmdMatches(const char *text, int state) {
    static int idx, len;
    const char *name;
    Jsi_Interp* interp = jsi_interactiveInterp;
    if (completeValues == NULL || !Jsi_ValueIsArray(interp, completeValues))
        return NULL;
    Jsi_Value **arr = completeValues->d.obj->arr;
    int aLen = completeValues->d.obj->arrCnt;

    if (!state)
    {
        idx = 0;
        len = Jsi_Strlen(text)-jsiRlStart;
    }
    while (idx<aLen)
    {
        name = Jsi_ValueString(interp, arr[idx], NULL);
        if (!name) name = "";
        idx++;
        if (Jsi_Strncmp(name, text+jsiRlStart, len) == 0)
            return (Jsi_Strdup(name));
    }
    return NULL;
}
static void jsiLNGetMatches(const char *str, linenoiseCompletions *lc) {
    char buf[JSI_BUFSIZ], pre[JSI_BUFSIZ], hpre[6] = {};
    const char *cp, *fnam = "Info.completions";
    int i = 0, len;
    int rc, isfile = 0, start = 0, end = Jsi_Strlen(str);
    Jsi_Interp* interp = jsi_interactiveInterp;
    if (!Jsi_Strncmp(str, "help ", 5)) {
        Jsi_Strcpy(hpre, "help ");
        str += 5;
        end -= 5;
    }
    if (end<=0) return;
    Jsi_Strncpy(buf, str, sizeof(buf)-1);
    buf[sizeof(buf)-1] = 0;
    pre[0] = 0;
    if (end<=3 && !Jsi_Strncmp(str, "help", end)) {
        linenoiseAddCompletion(lc, "help");
        return;
    }
    if (!completeValues)
        completeValues = Jsi_ValueNew1(interp);
    Jsi_Value *func = interp->onComplete;
    if (func == NULL || !Jsi_ValueIsFunction(interp, func)) {
        for (i=0; jsiFilePreCmds[i]; i++)
            if (!Jsi_Strncmp(buf, jsiFilePreCmds[i], Jsi_Strlen(jsiFilePreCmds[i]))) break;
        if (jsiFilePreCmds[i] && ((cp=Jsi_Strrchr(buf, '(')) && (cp[1]=='\"' || cp[1]=='\''))) {
            Jsi_Strcpy(pre, buf);
            pre[cp-buf+2] = 0;
            snprintf(buf, sizeof(buf), "%s*%s", cp+2, (buf[0]=='s'?".js*":""));
            isfile = 1;
            fnam = "File.glob";
        }
    }
    func = Jsi_NameLookup(interp, fnam);
    if (func && Jsi_ValueIsFunction(interp, func)) {
        //printf("PATTERN: %s\n", str);
        Jsi_Value *items[3] = {};;
        i = 0;
        items[i++] = Jsi_ValueNewStringDup(interp, buf);
        if (!isfile) {
            items[i++] = Jsi_ValueNewNumber(interp, (Jsi_Number)start);
            items[i++] = Jsi_ValueNewNumber(interp, (Jsi_Number)end);
        }
        Jsi_Value *args = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, i, 0));
        Jsi_IncrRefCount(interp, args);
        rc = Jsi_FunctionInvoke(interp, func, args, &completeValues, interp->csc);
        Jsi_DecrRefCount(interp, args);
        if (rc != JSI_OK) {
            fprintf(stderr, "bad completion: %s %d %d\n", str?str:"", start, end);
            return;
        }
        const char *name;
        Jsi_Interp* interp = jsi_interactiveInterp;
        if (completeValues == NULL || !Jsi_ValueIsArray(interp, completeValues))
            return;
        Jsi_Value **arr = completeValues->d.obj->arr;
        int aLen = completeValues->d.obj->arrCnt;
        i = 0;
        while (i<aLen)
        {
            name = Jsi_ValueString(interp, arr[i], &len);
            if (name) {
                if (!pre[0] && !hpre[0])
                    linenoiseAddCompletion(lc, name);
                else {
                    snprintf(buf, sizeof(buf), "%s%s%s", hpre, pre, name);
                    linenoiseAddCompletion(lc, buf);
                }
            }
            i++;
        }
    }
}
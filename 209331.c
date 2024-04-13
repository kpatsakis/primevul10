static char **jsiRlGetMatches(const char *cstr, int start, int end) {
    char **matches = NULL;
    char *str = rl_line_buffer;
    jsiRlStart = start;
    if (1 || start == 0 || !completeValues) {
        int rc;
        Jsi_Interp* interp = jsi_interactiveInterp;
        if (!completeValues)
            completeValues = Jsi_ValueNew1(interp);
        Jsi_Value *func = interp->onComplete;
        if (func == NULL || !Jsi_ValueIsFunction(interp, func))
            func = Jsi_NameLookup(interp, "Info.completions");
        if (func && Jsi_ValueIsFunction(interp, func)) {
            Jsi_Value *items[3] = {};
            items[0] = Jsi_ValueNewStringDup(interp, str);
            items[1] = Jsi_ValueNewNumber(interp, (Jsi_Number)start);
            items[2] = Jsi_ValueNewNumber(interp, (Jsi_Number)end);;
            Jsi_Value *args = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 3, 0));
            Jsi_IncrRefCount(interp, args);
            rc = Jsi_FunctionInvoke(interp, func, args, &completeValues, interp->csc);
            Jsi_DecrRefCount(interp, args);
            if (rc != JSI_OK)
                fprintf(stderr, "bad completion: %s %d %d\n", str?str:"", start, end);
        }
        matches = rl_completion_matches(str, jsiRlCmdMatches);
    }
    return matches;
}
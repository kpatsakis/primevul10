const char *jsiFuncInfo(Jsi_Interp *interp, Jsi_DString *dStr, Jsi_Func* func, Jsi_Value *arg) {
    if (!func) return "";
    if (func->name)
        Jsi_DSPrintf(dStr, ", in call to '%s'", func->name);
    else
        Jsi_DSPrintf(dStr, ", in call to function");
    if (func->script) {
        const char *cp = Jsi_Strrchr(func->script, '/');
        if (cp)
            cp++;
        else
            cp = func->script;
        Jsi_DSPrintf(dStr, " declared at %s:%d.%d", cp, func->bodyline.first_line, func->bodyline.first_column);
    }
        if (arg) {
        Jsi_DSAppend(dStr, " <", NULL);
        Jsi_ValueGetDString(interp, arg, dStr, 0);
        Jsi_DSAppend(dStr, ">.", NULL);
    }
    return Jsi_DSValue(dStr);
}
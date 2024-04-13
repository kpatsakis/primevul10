bool jsi_FuncArgCheck(Jsi_Interp *interp, Jsi_Func *f, const char *argStr)
{
    int i, atyp, ftyp, rc = 0, acnt;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    int argc = 0;
    char **argv, *sname, *stype, *cp;
    if (!argStr)
        goto done;
    if (f->type == FC_BUILDIN) {
        // Check builtin cmd
        jsi_CommandArgCheck(interp, f->cmdSpec, f, f->parentName);
        goto done;
    }
    if ((cp=Jsi_Strchr(argStr, '='))) {
        Jsi_LogWarn("may not have default value in option, expected: %s", argStr);
        goto done;
    }
    if (Jsi_Strstr(argStr, "...")) {
        Jsi_LogWarn("may not have ... in args, expected: %s", argStr);
        goto done;
    }
    if (argStr[0]) {
        Jsi_SplitStr(argStr, &argc, &argv, ",", &dStr);
        if (argc<=0)
            goto done;
    }
    if (!f->argnames) {
        if (argStr[0])
            Jsi_LogWarn("function has no args, expected: %s", argStr);
        else
            rc = 1;
        goto done;
    } else {
        if (f->argnames->varargs) { // TODO: could allow varargs...
            if (argc < f->argnames->argCnt) {
                Jsi_LogWarn("vararg argument mismatch, expected: %s", argStr);
                goto done;
            }
        }
        else if (f->argnames->argCnt != argc) {
            if (argc)
                Jsi_LogWarn("argument mismatch, expected: %s", argStr);
            else
                Jsi_LogWarn("function should have no arguments");
            goto done;
        }

    }
    acnt = f->argnames->argCnt;
    for (i=0; i<argc && i<acnt; i++) {
        sname = argv[i];
        stype = NULL;
        while (sname && *sname && isspace(*sname)) { sname++; }
        if ((cp=Jsi_Strchr(sname, ':')))
        {
            stype = cp+1;
            *cp = 0;
            while (*stype && isspace(*stype)) { stype++; }
            if (*stype) {
                cp = stype+Jsi_Strlen(stype)-1;
                while (cp>=stype && isspace(*cp)) { *cp = 0; cp--; }
            }
        }
        if (sname && *sname) {
            cp = sname+Jsi_Strlen(sname)-1;
            while (cp>=sname && isspace(*cp)) { *cp = 0; cp--; }
        }
        ftyp = f->argnames->args[i].type;
        if (ftyp<=0 || (ftyp&JSI_TT_ANY))
            continue;
        atyp = jsi_typeGet(interp, stype);
        if (ftyp != atyp && atyp) {
            Jsi_LogWarn("argument %d of function \"%s\" does not match \"func(%s)\"" ,
                i+1, f->name, argStr);
            goto done;
        }
    }
    rc = 1;
done:
    Jsi_DSFree(&dStr);
    if (!rc)
        jsi_TypeMismatch(interp);
    return rc;
}
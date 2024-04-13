void jsi_FuncCallCheck(jsi_Pstate *p, jsi_Pline *line, int argc, bool isNew, const char *name, const char *namePre, Jsi_OpCodes *argCodes)
{
    Jsi_Interp *interp = p->interp;
    if (name == NULL || !(interp->typeCheck.funcsig|interp->typeCheck.all|interp->typeCheck.parse))
        return;
    if (name && isdigit(name[0]))
        return;
    Jsi_Value *val;
    val = Jsi_NameLookup2(interp, name, namePre);
    Jsi_Func *f = NULL;
    if (val != NULL) {
        if (Jsi_ValueIsFunction(interp, val))
            f = val->d.obj->d.fobj->func;
    } else if (interp->staticFuncsTbl) {
        f = (Jsi_Func*)Jsi_HashGet(interp->staticFuncsTbl, (void*)name, 0);
    }
    if (f)
        jsi_RunFuncCallCheck(interp, f, argc, name, line, argCodes, 1);
    else if (interp->typeCheck.funcsig && (namePre==NULL || jsi_BuiltinCmd(interp, namePre))) {
        if (line)
            interp->parseLine = line;
        Jsi_LogWarn("called function '%s' with no previous definition", name);
        jsi_TypeMismatch(interp);
        if (line)
            interp->parseLine = NULL;
    }
}
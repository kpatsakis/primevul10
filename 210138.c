int jsi_FuncSigsMatch(jsi_Pstate *pstate, Jsi_Func *f1, Jsi_Func *f2)
{
    // Skip where both functions have no types.
    if (f1->retType==0 && f1->argnames->typeCnt==0 && f1->argnames->varargs==0 &&
        f2->retType==0 && f2->argnames->typeCnt==0 && f2->argnames->varargs==0 &&
        pstate->interp->typeCheck.all==0)
        return 1;
    if (f1->retType != f2->retType)
        return 0;
    if (f1->argnames->count != f2->argnames->count)
        return 0;
    if (f1->argnames->typeCnt != f2->argnames->typeCnt)
        return 0;
    if (f1->argnames->varargs != f2->argnames->varargs)
        return 0;
    int i;
    for (i=0; i<f1->argnames->count; i++) {
        Jsi_ScopeStrs *a1 = f1->argnames, *a2 = f2->argnames;
        if (a1->args[i].type != a2->args[i].type)
            return 0;
        Jsi_Value *v1, *v2;
        v1 = a1->args[i].defValue;
        v2 = a2->args[i].defValue;
        if (v1==NULL && v2 == NULL)
            continue;
        if (v1==NULL || v2 == NULL)
            return 0;
        if (v1->vt != v2->vt)
            return 0;
        if (Jsi_ValueCmp(pstate->interp, v1, v2, 0))
            return 0;
    }
    return 1;
}
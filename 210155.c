Jsi_ScopeStrs *jsi_argInsert(jsi_Pstate *pstate, Jsi_ScopeStrs *a, const char *name, Jsi_Value *defValue, jsi_Pline *lPtr, bool prepend)
{
    Jsi_Interp *interp = pstate->interp;
    if (!a)
        a = jsi_ScopeStrsNew();
    pstate->args = a;
    int atyp = pstate->argType;
    if (defValue) {
        int vt = defValue->vt;
        if (vt == JSI_VT_NULL)
            vt = JSI_TT_NULL;
        else if (vt == JSI_VT_UNDEF && defValue->d.num==1)
            vt = JSI_TT_VOID;
        else if (vt == JSI_VT_OBJECT && defValue->d.obj->ot==JSI_OT_STRING)
            vt = JSI_TT_STRING;
        else
            vt = (1<<defValue->vt);
        atyp |= vt;
    }
    jsi_ScopeStrsPush(interp, a, name, atyp);
    pstate->argType = 0;
    a->args[a->count-1].defValue = defValue;
    a->argCnt++;
    if (prepend) {
        jsi_ArgValue t;
        int i, end = a->argCnt-1;
        t = a->args[end];
        for (i=end-1; i>=0; i--)
            a->args[i+1]=a->args[i];
        a->args[0] = t;
    }
    jsi_Pline *opl = interp->parseLine;
    interp->parseLine = lPtr;
    if (defValue) {
        Jsi_IncrRefCount(interp, defValue);
        if (a->firstDef==0)
            a->firstDef = a->argCnt;
            if (atyp)
                jsi_ArgTypeCheck(interp, atyp, defValue, "default value", name, a->argCnt, NULL, 1);
    } else {
        if (a->firstDef && (interp->typeCheck.run || interp->typeCheck.all) )
            Jsi_LogWarn("expected default value in argument list: \"%s\"", name);
    }
    interp->parseLine = opl;
    return a;
}
static Jsi_RC RegexpExecCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int skip = 0;
    Jsi_Value *v;
    ChkRegexp(_this, funcPtr, v);
    /* TODO: add lastIndex support. */
    int slen;
    char *str = Jsi_ValueString(interp,Jsi_ValueArrayIndex(interp, args, skip), &slen);
    if (!str) 
        return Jsi_LogError("expected string");
    if (v == NULL || v->vt != JSI_VT_OBJECT || v->d.obj->ot != JSI_OT_REGEXP) 
        return Jsi_LogError("expected pattern");
    Jsi_Regex *re = v->d.obj->d.robj;
    int isglob = (re->eflags&JSI_REG_GLOB);
    Jsi_Value *l = NULL;
    Jsi_Number lv = 0;
    if (isglob) {
        l = Jsi_ValueObjLookup(interp, v, "lastIndex", 0);
        if (l && Jsi_ValueGetNumber(interp, l, &lv) != JSI_OK) 
            return Jsi_LogError("lastIndex not a number");
        if (l)
            re->lastIndex = (int)lv;
    }
    int ofs = 0;
    Jsi_RC rc = jsi_RegExpMatches(interp, v, re->lastIndex<slen?str+re->lastIndex:"", -1, *ret, isglob?&ofs:NULL, 0);
    if (isglob) {
        if (rc != JSI_OK)
            return rc;
        re->lastIndex += ofs;
        if (Jsi_ValueIsNull(interp, *ret))
            re->lastIndex = 0;
        lv = (Jsi_Number)re->lastIndex;
        if (!l)
            Jsi_ValueInsert(interp, v, "lastIndex", Jsi_ValueNewNumber(interp, lv), JSI_OM_DONTDEL);
        else if (l->vt == JSI_VT_NUMBER)
            l->d.num = lv;
        else if (l->vt == JSI_VT_OBJECT && l->d.obj->ot == JSI_OT_NUMBER)
            l->d.obj->d.num = lv;
    }
    return rc;
}
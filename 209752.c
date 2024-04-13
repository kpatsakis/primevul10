Jsi_RC jsi_StaticArgTypeCheck(Jsi_Interp *interp, int atyp, const char *p1, const char *p2, int index, Jsi_Func *func, jsi_Pline *line) {
    Assert(index>0);
    Jsi_RC rc;
    if (interp->typeCheck.parse==0 && interp->typeCheck.all==0)
        return JSI_OK;
    int ai = index-1+func->callflags.bits.addargs;
    if (func->argnames == NULL || ai>=func->argnames->count || ai<0)
        return JSI_OK;
    int typ = func->argnames->args[ai].type;
    if (typ <= 0)
        return JSI_OK;
    if (index == 0 && func && func->type == FC_BUILDIN && 
        interp->typeCheck.all==0) // Normally do not check return types for builtins.
        return JSI_OK; 
    if ((typ&JSI_TT_ANY)) return JSI_OK;
    if (index == 0 && atyp == JSI_VT_UNDEF) {
        if (!(typ&JSI_TT_VOID)) 
            goto done;
        return JSI_OK;
    }
/*    if (index == 0 && (typ&JSI_TT_VOID)) {
        if (atyp != JSI_VT_UNDEF && !(typ&JSI_TT_UNDEFINED))
            goto done;
        return JSI_OK;
    }*/
    if (atyp == JSI_VT_UNDEF)
        return JSI_OK;
    rc = JSI_OK;
    if (typ&JSI_TT_UNDEFINED && atyp == JSI_TT_UNDEFINED) return rc;
    if (typ&JSI_TT_NUMBER && atyp==JSI_TT_NUMBER) return rc;
    if (typ&JSI_TT_STRING && atyp==JSI_TT_STRING) return rc;
    if (typ&JSI_TT_BOOLEAN && atyp==JSI_TT_BOOLEAN)  return rc;
    if (typ&JSI_TT_ARRAY && atyp==JSI_TT_ARRAY)   return rc;
    if (typ&JSI_TT_FUNCTION && atyp==JSI_TT_FUNCTION) return rc;
    if (typ&JSI_TT_REGEXP && atyp==JSI_TT_REGEXP) return rc;
    if (typ&JSI_TT_USEROBJ && atyp==JSI_TT_USEROBJ) return rc;
    if (typ&JSI_TT_ITEROBJ && atyp==JSI_TT_ITEROBJ) return rc;
    if (typ&JSI_TT_OBJECT && atyp==JSI_TT_OBJECT) return rc;
    if (typ&JSI_TT_NULL && atyp==JSI_TT_NULL) return rc;
done:
    {
        Jsi_DString dStr = {};
        const char *exp = jsi_typeName(interp, typ, &dStr);
        const char *vtyp = jsi_TypeName(interp, (Jsi_ttype)atyp);
        char idxBuf[JSI_MAX_NUMBER_STRING*2];
        idxBuf[0] = 0;
        if (index>0)
            snprintf(idxBuf, sizeof(idxBuf), " arg %d", index);
        if (line)
            interp->parseLine = line;
        if (interp->typeCheck.error)
            rc = JSI_ERROR;
        jsi_TypeMismatch(interp);
        Jsi_DString fStr = {};
        rc = Jsi_LogType("type mismatch %s%s '%s': expected \"%s\" but got \"%s\"%s",
            p1, idxBuf, p2, exp, vtyp, jsiFuncInfo(interp, &fStr, func, NULL));
        Jsi_DSFree(&fStr);
        Jsi_DSFree(&dStr);
    }
    return rc;
}
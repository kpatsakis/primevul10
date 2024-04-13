Jsi_RC jsi_ArgTypeCheck(Jsi_Interp *interp, int typ,  Jsi_Value *arg, const char *p1,
    const char *p2, int index, Jsi_Func *func, bool isdefault) {
    Jsi_RC rc = JSI_OK;
    char idxBuf[JSI_MAX_NUMBER_STRING*2];
    idxBuf[0] = 0;
    if (func && arg->vt == JSI_VT_UNDEF && !interp->typeCheck.noundef && index>0 && !isdefault && !(typ&JSI_TT_UNDEFINED)) {
        snprintf(idxBuf, sizeof(idxBuf), " arg %d", index);
        jsi_TypeMismatch(interp);
       
        Jsi_DString fStr = {};
        rc = Jsi_LogType("call with undefined var %s%s '%s'%s", p1, idxBuf, p2, jsiFuncInfo(interp, &fStr, func, arg));
        Jsi_DSFree(&fStr);
        return rc;
    }
    if (typ <= 0)
        return JSI_OK;
    //if (typ&JSI_TT_VOID)
    //    return JSI_OK;
    if (interp->typeCheck.all==0) {
        if (func ? (interp->typeCheck.run==0) : (interp->typeCheck.parse==0))
            return JSI_OK;
    }
    if (index == 0 && func && func->type == FC_BUILDIN && 
        interp->typeCheck.all == 0) // Normally do not check return types for builtins.
        return JSI_OK; 
    if ((typ&JSI_TT_ANY)) return JSI_OK;
    if (index == 0 && arg->vt == JSI_VT_UNDEF) {
        if (!(typ&JSI_TT_VOID)) 
            goto done;
        return JSI_OK;
    }
    if (isdefault && index && typ&JSI_TT_VOID && arg->vt == JSI_VT_UNDEF)
        return JSI_OK;
    if (typ&JSI_TT_UNDEFINED && Jsi_ValueIsUndef(interp, arg)) return rc;
    if (typ&JSI_TT_NUMBER && Jsi_ValueIsNumber(interp, arg)) return rc;
    if (typ&JSI_TT_STRING && Jsi_ValueIsString(interp, arg)) return rc;
    if (typ&JSI_TT_BOOLEAN && Jsi_ValueIsBoolean(interp, arg))  return rc;
    if (typ&JSI_TT_ARRAY && Jsi_ValueIsArray(interp, arg))   return rc;
    if (typ&JSI_TT_FUNCTION && Jsi_ValueIsFunction(interp, arg)) return rc;
    if (typ&JSI_TT_REGEXP && Jsi_ValueIsObjType(interp, arg, JSI_OT_REGEXP)) return rc;
    if (typ&JSI_TT_USEROBJ && Jsi_ValueIsObjType(interp, arg, JSI_OT_USEROBJ)) return rc;
    if (typ&JSI_TT_ITEROBJ && Jsi_ValueIsObjType(interp, arg, JSI_OT_ITER)) return rc;
    if (typ&JSI_TT_OBJECT && (
        Jsi_ValueIsObjType(interp, arg, JSI_OT_OBJECT) && Jsi_ValueIsArray(interp, arg)==0))
        return rc;
    if (typ&JSI_TT_NULL && Jsi_ValueIsNull(interp, arg)) return rc;
done:
    {
        Jsi_DString dStr = {};
        const char *exp = jsi_typeName(interp, typ, &dStr);
        const char *vtyp = jsi_ValueTypeName(interp, arg);
        if (index>0)
            snprintf(idxBuf, sizeof(idxBuf), " arg %d", index);
        if (interp->typeCheck.error)
            rc = JSI_ERROR;
        jsi_TypeMismatch(interp);
        Jsi_DString fStr = {};
        rc = Jsi_LogType("type mismatch %s%s '%s': expected \"%s\" but got \"%s\"%s",
            p1, idxBuf, p2, exp, vtyp, jsiFuncInfo(interp, &fStr, func, arg));
        Jsi_DSFree(&fStr);
        Jsi_DSFree(&dStr);
    }
    return rc;
}
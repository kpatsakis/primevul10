void *Jsi_CommandNewObj(Jsi_Interp *interp, const char *name, const char *arg1, const char *opts, const char *var) {
    char buf[JSI_BUFSIZ];
    if (arg1)
        snprintf(buf, sizeof(buf), "%s%snew %s('%s', %s);", var?var:"", var?"=":"return ", name, arg1, opts?opts:"null");
    else
        snprintf(buf, sizeof(buf), "%s%snew %s(%s);", var?var:"", var?"=":"return ", name, opts?opts:"null");
    int rc = Jsi_EvalString(interp, buf, 0);
    if (rc != JSI_OK)
        return NULL;
    Jsi_Value *vObj = interp->retValue;
    if (var)
        vObj = Jsi_NameLookup(interp, var);
    if (!vObj)
        return NULL;
    return Jsi_UserObjGetData(interp, vObj, NULL);
}
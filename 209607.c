const char *jsi_typeName(Jsi_Interp *interp, int typ, Jsi_DString *dStr) {
    if (typ<=0 || (typ&JSI_TT_ANY)) {
        Jsi_DSAppend(dStr, "any", NULL);
        return Jsi_DSValue(dStr);
    }
    int i = 0;
    if (typ&JSI_TT_NUMBER) Jsi_DSAppend(dStr, (i++?"|":""), "number", NULL);
    if (typ&JSI_TT_STRING) Jsi_DSAppend(dStr, (i++?"|":""), "string", NULL);
    if (typ&JSI_TT_BOOLEAN)  Jsi_DSAppend(dStr, (i++?"|":""), "boolean", NULL);
    if (typ&JSI_TT_ARRAY)   Jsi_DSAppend(dStr, (i++?"|":""), "array", NULL);
    if (typ&JSI_TT_FUNCTION) Jsi_DSAppend(dStr, (i++?"|":""), "function", NULL);
    if (typ&JSI_TT_OBJECT) Jsi_DSAppend(dStr, (i++?"|":""), "object", NULL);
    if (typ&JSI_TT_REGEXP) Jsi_DSAppend(dStr, (i++?"|":""), "regexp", NULL);
    if (typ&JSI_TT_USEROBJ) Jsi_DSAppend(dStr, (i++?"|":""), "userobj", NULL);
    if (typ&JSI_TT_ITEROBJ) Jsi_DSAppend(dStr, (i++?"|":""), "iterobj", NULL);
    if (typ&JSI_TT_NULL) Jsi_DSAppend(dStr, (i++?"|":""), "null", NULL);
    if (typ&JSI_TT_UNDEFINED) Jsi_DSAppend(dStr, (i++?"|":""), "undefined", NULL);
    if (typ&JSI_TT_VOID) Jsi_DSAppend(dStr, (i++?"|":""), "void", NULL);
    return Jsi_DSValue(dStr);
}
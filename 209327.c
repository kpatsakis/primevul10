const char *jsi_ObjectTypeName(Jsi_Interp *interp, Jsi_otype otyp)
{
    switch (otyp) {
        case JSI_OT_NUMBER:     return "number"; 
        case JSI_OT_STRING:     return "string"; 
        case JSI_OT_BOOL:       return "boolean"; 
        case JSI_OT_ARRAY:      return "array"; 
        case JSI_OT_FUNCTION:   return "function"; 
        case JSI_OT_OBJECT:     return "object"; 
        case JSI_OT_REGEXP:     return "regexp"; 
        case JSI_OT_ITER:       return "iter"; 
        case JSI_OT_USEROBJ:    return "userobj"; 
        case JSI_OT_UNDEF:      return "any";
    }
    return "undefined";
}
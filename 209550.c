const char *jsi_TypeName(Jsi_Interp *interp, Jsi_ttype otyp)
{
    switch (otyp) {
        case JSI_TT_NUMBER:     return "number"; 
        case JSI_TT_STRING:     return "string"; 
        case JSI_TT_BOOLEAN:    return "boolean"; 
        case JSI_TT_ARRAY:      return "array"; 
        case JSI_TT_FUNCTION:   return "function"; 
        case JSI_TT_OBJECT:     return "object"; 
        case JSI_TT_REGEXP:     return "regexp"; 
        case JSI_TT_ANY:        return "any"; 
        case JSI_TT_USEROBJ:    return "userobj"; 
        case JSI_TT_ITEROBJ:    return "iterobj"; 
        case JSI_TT_UNDEFINED:      return "undefined";
        case JSI_TT_VOID:       return "void";
        case JSI_TT_NULL:       return "null"; 
    }
    return "undefined";
}
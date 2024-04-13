const char *jsi_ValueTypeName(Jsi_Interp *interp, Jsi_Value *val)
{
    switch (val->vt) {
        case JSI_VT_NUMBER:     return "number"; 
        case JSI_VT_STRING:     return "string"; 
        case JSI_VT_BOOL:       return "boolean"; 
        case JSI_VT_OBJECT:     if (val->d.obj->ot == JSI_OT_OBJECT && val->d.obj->isarrlist) return "array"; return jsi_ObjectTypeName(interp, val->d.obj->ot); 
        case JSI_VT_VARIABLE:   return "variable"; 
        case JSI_VT_NULL:       return "null"; 
        case JSI_VT_UNDEF:      break;
    }
    return "undefined";
}
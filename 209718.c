const char *Jsi_ValueTypeStr(Jsi_Interp *interp, Jsi_Value *v)
{
    switch (v->vt) {
        case JSI_VT_BOOL: return "boolean"; break;
        case JSI_VT_UNDEF: return "undefined"; break;
        case JSI_VT_NULL: return "null"; break;
        case JSI_VT_NUMBER: return "number"; break;
        case JSI_VT_STRING: return "string"; break;  
        case JSI_VT_VARIABLE: return "variable"; break;  
        case JSI_VT_OBJECT: return Jsi_ObjTypeStr(interp, v->d.obj);
    }
    return "";
}
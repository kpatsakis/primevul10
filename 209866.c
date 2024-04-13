const char *Jsi_ObjTypeStr(Jsi_Interp *interp, Jsi_Obj *o)
{
     switch (o->ot) {
        case JSI_OT_BOOL: return "boolean"; break;
        case JSI_OT_FUNCTION: return "function"; break;
        case JSI_OT_NUMBER: return "number"; break;
        case JSI_OT_STRING: return "string"; break;  
        case JSI_OT_REGEXP: return "regexp"; break;  
        case JSI_OT_ITER: return "iter"; break;  
        case JSI_OT_OBJECT: if (!o->isarrlist) return "object";
        case JSI_OT_ARRAY: return "array"; break;  
        case JSI_OT_USEROBJ:
            if (o->__proto__) {
                Jsi_HashEntry *hPtr;
                Jsi_HashSearch search;
                            
                for (hPtr = Jsi_HashSearchFirst(interp->thisTbl,&search); hPtr != NULL;
                    hPtr = Jsi_HashSearchNext(&search))
                    if (Jsi_HashValueGet(hPtr) == o->__proto__)
                        return (char*)Jsi_HashKeyGet(hPtr);
            }
            
            return "userobj";
            break;
            //return Jsi_ObjGetType(interp, v->d.obj);
        default:
            break;
     }
     return "";
}
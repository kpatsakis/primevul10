Jsi_Number Jsi_ValueToNumberInt(Jsi_Interp *interp, Jsi_Value *v, int isInt)
{
    char *endPtr = NULL, *sptr;
    Jsi_Number a = 0;
    switch(v->vt) {
        case JSI_VT_BOOL:
            a = (Jsi_Number)(v->d.val ? 1.0: 0);
            break;
        case JSI_VT_NULL:
            a = 0;
            break;
        case JSI_VT_OBJECT: {
            Jsi_Obj *obj = v->d.obj;
            switch(obj->ot) {
                case JSI_OT_BOOL:
                    a = (Jsi_Number)(obj->d.val ? 1.0: 0);
                    break;
                case JSI_OT_NUMBER:
                    a = obj->d.num;
                    break;
                case JSI_OT_STRING:
                    sptr = obj->d.s.str;
                    goto donum;
                    break;
                default:
                    a = 0;
                break;
            }
            break;
        }
        case JSI_VT_UNDEF:
            a = Jsi_NumberNaN();
            break;
        case JSI_VT_NUMBER:
            a = v->d.num;
            break;
        case JSI_VT_STRING:
            sptr = v->d.s.str;
donum:
            if (!isInt) {
                a = strtod(sptr, &endPtr);
                if (endPtr && *endPtr) {
                    a = interp->NaNValue->d.num;
                }
            } else {
                a = (Jsi_Number)strtol(sptr, &endPtr, 0);
                if (!isdigit(*sptr))
                    a = interp->NaNValue->d.num;
            }
            break;
        default:
            Jsi_LogBug("Convert a unknown type: 0x%x to number", v->vt);
            break;
    }
    if (isInt && Jsi_NumberIsNormal(a))
        a = (Jsi_Number)((int64_t)(a));
    return a;
}
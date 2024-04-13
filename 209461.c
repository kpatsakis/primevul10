Jsi_RC Jsi_ValueToBool(Jsi_Interp *interp, Jsi_Value *v)
{
    Jsi_RC rc = JSI_OK;
    bool a = 0;
    switch(v->vt) {
        case JSI_VT_BOOL:
            a = v->d.val;
            break;
        case JSI_VT_NULL:
            break;
        case JSI_VT_UNDEF:
            break;
        case JSI_VT_NUMBER:
            a = (v->d.num ? 1: 0);
            break;
        case JSI_VT_STRING:     /* TODO: NaN, and accept true/false string? */
            a = atoi(v->d.s.str);
            a = (a ? 1 : 0);
            break;
        case JSI_VT_OBJECT: {
            Jsi_Obj *obj = v->d.obj;
            switch(obj->ot) {
                case JSI_OT_BOOL:
                    a = (obj->d.val ? 1.0: 0);
                    break;
                case JSI_OT_NUMBER:
                    a = obj->d.num;
                    a = (a ? 1 : 0);
                    break;
                case JSI_OT_STRING:
                    a = atoi(obj->d.s.str);
                    a = (a ? 1 : 0);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            Jsi_LogBug("Convert a unknown type: 0x%x to number", v->vt);
            return JSI_ERROR;
    }
    Jsi_ValueReset(interp,&v);
    Jsi_ValueMakeBool(interp, &v, a);
    return rc;
}
Jsi_RC Jsi_ValueToObject(Jsi_Interp *interp, Jsi_Value *v)
{
    Jsi_RC rc = JSI_OK;
    if (v->vt == JSI_VT_OBJECT) return rc;
    Jsi_Obj *o = Jsi_ObjNew(interp);
    switch(v->vt) {
        case JSI_VT_UNDEF:
        case JSI_VT_NULL:
            if (interp->strict) {
                Jsi_LogError("converting a undefined/null value to object");
                rc = JSI_ERROR;
            }
            o->d.num = 0;
            o->ot = JSI_OT_NUMBER;
            o->__proto__ = interp->Number_prototype;
            break;
        case JSI_VT_BOOL: {
            o->d.val = v->d.val;
            o->ot = JSI_OT_BOOL;
            o->__proto__ = interp->Boolean_prototype;
            break;
        }
        case JSI_VT_NUMBER: {
            o->d.num = v->d.num;
            o->ot = JSI_OT_NUMBER;
            o->__proto__ = interp->Number_prototype;
            break;
        }
        case JSI_VT_STRING: {
            o->d.s = v->d.s;
            if (!v->f.bits.isstrkey)
                o->d.s.str = (char*)Jsi_KeyAdd(interp, v->d.s.str);
            o->isstrkey = 1;
            o->ot = JSI_OT_STRING;
            o->__proto__ = interp->String_prototype;
            break;
        }
        default:
            Jsi_LogBug("toobject, not suppose to reach here");
    }
    Jsi_ValueReset(interp,&v);
    Jsi_ValueMakeObject(interp, &v, o);
    return rc;
}
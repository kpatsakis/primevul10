Jsi_RC Jsi_CleanValue(Jsi_Interp *interp, Jsi_Interp *tointerp, Jsi_Value *val, Jsi_Value **ret)
{
    Jsi_RC rc = JSI_OK;
    const char *cp;
    int len, iskey;
    Jsi_Obj *obj;
    switch (val->vt) {
        case JSI_VT_UNDEF: Jsi_ValueMakeUndef(interp, ret); return rc;
        case JSI_VT_NULL: Jsi_ValueMakeNull(tointerp, ret); return rc;
        case JSI_VT_BOOL: Jsi_ValueMakeBool(tointerp, ret, val->d.val); return rc;
        case JSI_VT_NUMBER: Jsi_ValueMakeNumber(tointerp, ret, val->d.num); return rc;
        case JSI_VT_STRING:
            iskey = val->f.bits.isstrkey;
            cp = val->d.s.str;
            len = val->d.s.len;
makestr:
            if (iskey) {
                Jsi_ValueMakeStringKey(interp, ret, cp);
                return rc;
            }
            jsi_ValueMakeBlobDup(tointerp, ret, (uchar*)cp, len);
            return rc;
        case JSI_VT_OBJECT:
            obj = val->d.obj;
            switch (obj->ot) {
                case JSI_OT_BOOL: Jsi_ValueMakeBool(tointerp, ret, obj->d.val); return rc;
                case JSI_OT_NUMBER: Jsi_ValueMakeNumber(tointerp, ret, obj->d.num); return rc;
                case JSI_OT_STRING:
                    cp = obj->d.s.str;
                    len = obj->d.s.len;
                    iskey = obj->isstrkey;
                    goto makestr;
                default: break;
            }
            break;
        default:
            break;
    }
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    cp = Jsi_ValueGetDString(interp, val, &dStr, JSI_OUTPUT_JSON);
    if (Jsi_JSONParse(tointerp, cp, ret, 0) != JSI_OK) {
        Jsi_DSFree(&dStr);
        return Jsi_LogWarn("bad JSON parse in subinterp");
    }
    Jsi_DSFree(&dStr);
    return rc;
}
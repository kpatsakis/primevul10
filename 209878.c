const char* Jsi_ValueToString(Jsi_Interp *interp, Jsi_Value *v, int *lenPtr)
{
    Jsi_Number d;
    const char *ntxt = "undefined";
    int kflag = 1;
    int isKey = 0;
    char *key = NULL;
    if (!v)
        goto done;
    if (lenPtr) *lenPtr = 0;
    char unibuf[JSI_MAX_NUMBER_STRING*2];
    switch(v->vt) {
        case JSI_VT_STRING:
            ntxt = v->d.s.str;
            goto done;
        case JSI_VT_UNDEF:
            break;
        case JSI_VT_BOOL:
            ntxt = v->d.val ? "true":"false";
            break;
        case JSI_VT_NULL:
            ntxt = "null";
            break;
        case JSI_VT_NUMBER: {
            d = v->d.num;
fmtnum:
            if (Jsi_NumberIsInteger(d)) {
                Jsi_NumberItoA10((Jsi_Wide)d, unibuf, sizeof(unibuf));
                kflag = 0;
                ntxt = unibuf;
            } else if (Jsi_NumberIsNormal(d)) {
                Jsi_NumberDtoA(interp, d, unibuf, sizeof(unibuf), 0);
                kflag = 0;
                ntxt = unibuf;
            } else if (Jsi_NumberIsNaN(v->d.num)) {
                ntxt = "NaN";
            } else {
                int s = Jsi_NumberIsInfinity(d);
                if (s > 0) ntxt = "Infinity";
                else if (s < 0) ntxt = "-Infinity";
                else Jsi_LogBug("Ieee function got problem");
            }
            break;
        }
        case JSI_VT_OBJECT: {
            Jsi_Obj *obj = v->d.obj;
            switch(obj->ot) {
                case JSI_OT_STRING:
                    ntxt = obj->d.s.str;
                    goto done;
                case JSI_OT_BOOL:
                    ntxt = obj->d.val ? "true":"false";
                    break;
                case JSI_OT_NUMBER:
                    d = obj->d.num;
                    goto fmtnum;
                    break;
                default:
                    ntxt = "[object Object]";
                    break;
            }
            break;
        }
        default:
            Jsi_LogBug("Convert a unknown type: 0x%x to string", v->vt);
            break;
    }
    Jsi_ValueReset(interp, &v);
    if (!kflag) {
        Jsi_ValueMakeStringDup(interp, &v, ntxt);
        return Jsi_ValueString(interp, v, lenPtr);
    }
    
    key = jsi_KeyFind(interp, ntxt, 0, &isKey);
    if (key)
        Jsi_ValueMakeStringKey(interp, &v, key);
    else
        Jsi_ValueMakeString(interp, &v, ntxt);
    ntxt = v->d.s.str;
    
done:
    if (lenPtr) *lenPtr = Jsi_Strlen(ntxt);
    return ntxt;
}
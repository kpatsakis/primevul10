jsonGen1Value(Jsi_Interp *interp, Jsi_JsonParser *p, const char *js, uint i, uint *endPos, int incr)
{
    uint len;
    const char *t;
    Jsi_Value *v = NULL;
    
    switch (p->tokens[i].type) {
        case JSI_JTYPE_PRIMITIVE:
            t = Jsi_JsonGetTokstr(p, js, i, &len);
            if (len == 4 && Jsi_Strncmp(t, "true", len)==0)
                v = Jsi_ValueMakeBool(interp, NULL, 1);
            else if (len == 5 && Jsi_Strncmp(t, "false", len)==0)
                v = Jsi_ValueMakeBool(interp, NULL, 0);
            else if (len == 4 && Jsi_Strncmp(t, "null", len)==0)
                v = Jsi_ValueMakeNull(interp, NULL);
            else if (len == 9 && Jsi_Strncmp(t, "undefined", len)==0)
                v = Jsi_ValueMakeNull(interp, NULL);
            else {
                char *ep;
                Jsi_Number d;
                d = strtod(t,&ep);
                if (ep>(t+len))
                    Jsi_LogWarn("bad number %*s", len, t);
                v = Jsi_ValueMakeNumber(interp, NULL, d);
            }
            break;
        case JSI_JTYPE_STRING:
            t = Jsi_JsonGetTokstr(p, js, i, &len);
            v = jsonNewStringObj(interp, t, len);
            break;
        case JSI_JTYPE_ARRAY:
            v = jsonGenArray(interp, p, js, i, &i);
            i--;
            break;
        case JSI_JTYPE_OBJECT:
            v = jsonGenObject(interp, p, js, i, &i);
            i--;
            break;
        default:
            break;
    }
    if (endPos)
        *endPos = i;
    if (v == NULL)
        v = Jsi_ValueMakeUndef(interp, NULL);
    if (incr)
        Jsi_IncrRefCount(interp, v);
    return v;
}
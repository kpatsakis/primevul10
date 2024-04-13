int Jsi_OptionsProcessJSON(Jsi_Interp *interp, Jsi_OptionSpec *opts, void *data, const char *json, Jsi_Wide flags)
{
    uint i = 0, r, len;
    Jsi_RC rc = JSI_OK;
    int result = 0;
    int strict = (flags & JSI_JSON_STRICT);
    Jsi_JsonTok *tok;
    const char *err;
    Jsi_Value *v = NULL;
    Jsi_DString nStr;
    const char *t, *name = NULL;
    if (json == NULL)
        return 0;
    while (isspace(*json))
        json++;
    if (json[0] == 0)
        return 0;
        
    JSI_JSON_DECLARE(pp, tokens, 0);
    Jsi_JsonParser *p = &pp;
    pp.strict = strict;
    pp.flags = flags;

    r = Jsi_JsonParse(p, json);
    if (r != JSI_JSON_ERR_NONE) {
        int ofs = pp.pos, len = Jsi_Strlen(json);
        if (ofs<0 || ofs>len)
            ofs = 0;
        err = p->errStr;
        if (!err)
            err = Jsi_JsonGetErrname(r);
        Jsi_LogError("JSON parse error (%s) at offset %d \"%.30s\"", err, ofs, json+ofs);
        result = -1;
        goto done;
    }
    tok = p->tokens;
    Jsi_DSInit(&nStr);
    if (tok->size%2 || tok->type != JSI_JTYPE_OBJECT) {
        result = -1;
        goto done;
    }
    v = Jsi_ValueNew1(interp);
    for (i=1; i<p->toknext && i<tok->size; i++) {
        if (p->tokens[i].type != JSI_JTYPE_STRING) {
            result = Jsi_LogError("expected string at %d", i-1);
            goto bail;
        }
        name = Jsi_JsonGetTokstr(p, json, i, &len);
        Jsi_DSSetLength(&nStr, 0);
        Jsi_DSAppendLen(&nStr, name, len);
        name = Jsi_DSValue(&nStr);
        i++;
        switch (p->tokens[i].type) {
            case JSI_JTYPE_PRIMITIVE:
                t = Jsi_JsonGetTokstr(p, json, i, &len);
                if ((len == 4 && Jsi_Strncmp(t, "true", len)==0) || (len == 5 && Jsi_Strncmp(t, "false", len)==0)) {
                    Jsi_ValueMakeBool(interp, &v, (bool)(len==4?1:0));
                } else if (len == 4 && Jsi_Strncmp(t, "null", len)==0) {
                    Jsi_ValueMakeNull(interp, &v);
                } else {
                    char *ep;
                    Jsi_Number d = strtod(t,&ep);
                    if (ep>(t+len)) {
                        result = Jsi_LogError("bad number %*s", len, t);
                        goto bail;
                    }
                    Jsi_ValueMakeNumber(interp, &v, d);
                }
                break;
            case JSI_JTYPE_STRING:
                t = Jsi_JsonGetTokstr(p, json, i, &len);
                Jsi_DString dStr;
                Jsi_DSInit(&dStr);
                jsonNewDString(interp, &dStr, t, len);
                Jsi_ValueMakeStringKey(interp, &v, Jsi_DSValue(&dStr));
                Jsi_DSFree(&dStr);
                break;
            case JSI_JTYPE_ARRAY:
            case JSI_JTYPE_OBJECT:
            default:
                result = -1;
                goto bail;
                break;
        }
        result++;
        rc = Jsi_OptionsSet(interp, opts, data, name, v, 0);
        if (rc == JSI_OK)
            continue;

bail:
        Jsi_LogError("bad at field: %s", name);
        result = -1;
        break;
    }

done:
    Jsi_DSFree(&nStr);
    if (v)
        Jsi_DecrRefCount(interp, v);
    Jsi_JsonFree(&pp);
    return result;
}
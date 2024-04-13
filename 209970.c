Jsi_RC Jsi_JSONParse(Jsi_Interp *interp, const char *js, Jsi_Value **ret, int flags)
{
    uint i = 0, r;
    Jsi_RC result = JSI_OK;
    int strict = (flags & JSI_JSON_STRICT);
    Jsi_JsonTok *tok;
    Jsi_Value *v;
    const char *err;
    Jsi_Number d;
    if (js == NULL)
        return JSI_OK;
    while (isspace(*js))
        js++;
    if (js[0] == 0)
        return JSI_OK;
    switch (js[0]) {
        case 't': if (Jsi_Strcmp(js,"true")==0) { if (ret) Jsi_ValueMakeBool(interp, ret, 1); return JSI_OK; } break;
        case 'f': if (Jsi_Strcmp(js,"false")==0) { if (ret) Jsi_ValueMakeBool(interp, ret, 0); return JSI_OK; } break;
        case 'n': if (Jsi_Strcmp(js,"null")==0) { if (ret) Jsi_ValueMakeNull(interp, ret); return JSI_OK; } break;
        case '0': case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9': case '-':
            if (Jsi_GetDouble(interp, js, &d) == JSI_OK) { if (ret) Jsi_ValueMakeNumber(interp, ret, d); return JSI_OK; } break;
    }
    JSI_JSON_DECLARE(pp, tokens, 0);
    Jsi_JsonParser *p = &pp;
    pp.strict = strict;
    pp.flags = flags;

    r = Jsi_JsonParse(p, js);
    if (r != JSI_JSON_ERR_NONE) {
        int ofs = pp.pos, len = Jsi_Strlen(js);
        if (ofs<0 || ofs>len)
            ofs = 0;
        err = p->errStr;
        if (!err)
            err = Jsi_JsonGetErrname(r);
        if (interp)
            Jsi_LogError("JSON parse error (%s) at offset %d \"%.30s\"", err, ofs, js+ofs);
        result = JSI_ERROR;
        goto done;
    }
    if (!ret)
        goto done;
    tok = p->tokens;
    if (tok->size<=0) {
        if (!*ret)
            *ret = Jsi_ValueNew1(interp);
        if (tok->type == JSI_JTYPE_OBJECT)
            Jsi_ValueMakeObject(interp, ret, Jsi_ObjNewObj(interp, NULL, 0));
        else if (tok->type == JSI_JTYPE_ARRAY)
            Jsi_ValueMakeArrayObject(interp, ret, Jsi_ObjNew(interp));
        else
            Jsi_ValueMakeUndef(interp, ret);
        goto done;
    }
    v = jsonGen1Value(interp, p, js, i, &i, 1);
    Jsi_ValueReplace(interp, ret, v);
    Jsi_DecrRefCount(interp, v);
done:
    Jsi_JsonFree(&pp);
    return result;
}
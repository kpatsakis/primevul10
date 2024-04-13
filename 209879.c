static Jsi_RC StringSearchCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{

    int sLen, bLen;
    const char *source_str;
    ChkString(_this, funcPtr, source_str, &sLen, &bLen);
    
    char *v = _this->d.obj->d.s.str;
    Jsi_Value *seq = Jsi_ValueArrayIndex(interp, args, skip);

    if (Jsi_ValueIsString(interp, seq)) {
        char *ce, *cp = Jsi_ValueString(interp, seq, NULL);
        int n = -1;
        if ((ce = Jsi_Strstr(source_str, cp))) {
            n = (ce-source_str);
        }
        Jsi_ValueMakeNumber(interp, ret, n);
        return JSI_OK;
    }
    if (!seq || seq->vt != JSI_VT_OBJECT || seq->d.obj->ot != JSI_OT_REGEXP) {
        Jsi_ValueMakeNumber(interp, ret, -1);
        return JSI_OK;
    }

    regex_t *reg = &seq->d.obj->d.robj->reg;
    
    regmatch_t pos[MAX_SUBREGEX] = {};
    int r;
    if ((r = regexec(reg, v, MAX_SUBREGEX, pos, 0)) != 0) {
        if (r == REG_NOMATCH) {
            Jsi_ValueMakeNumber(interp, ret, -1.0);
            return JSI_OK;
        }
        if (r >= REG_BADPAT) {
            char buf[JSI_MAX_NUMBER_STRING];

            regerror(r, reg, buf, sizeof(buf));
            Jsi_LogError("error while matching pattern: %s", buf);
            return JSI_ERROR;
        }

    }
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)pos[0].rm_so);
    return JSI_OK;
}
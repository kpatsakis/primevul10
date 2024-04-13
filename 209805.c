static Jsi_RC StringReplaceCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    /* Now handles perl regex flag extensions.*/
    const char *source_str;
    int source_len, bLen;
    const char *replace_str = NULL;
    int replace_len;
    int regexec_flags = 0;
    Jsi_Value *seq, *strVal;
    Jsi_DString dStr = {};
    regex_t *regex;
    Jsi_Regex *re;
    const char *p;
    int maxArgs = 1;

    int offset = 0, n, j, isglob = 0, num_matches = 0;
    /* Is a generic  String.replace if _this->d.obj is a function */
    ChkString(_this, funcPtr, source_str, &source_len, &bLen);
    source_len = bLen;
    if (!skip)
        strVal = _this;
    else
        strVal = Jsi_ValueArrayIndex(interp, args, 0);
    seq = Jsi_ValueArrayIndex(interp, args, skip);
    Jsi_Value *repVal = Jsi_ValueArrayIndex(interp, args, 1+skip);
    if (!Jsi_ValueIsFunction(interp, repVal))
        replace_str = Jsi_ValueToString(interp, repVal, &replace_len);
    else
        maxArgs = repVal->d.obj->d.fobj->func->argnames->argCnt;
    Jsi_DSInit(&dStr);

    if (Jsi_ValueIsString(interp, seq)) {
        const char *ce, *cp = Jsi_ValueString(interp, seq, NULL);
        if (!(ce = Jsi_Strstr(source_str, cp)))
            Jsi_ValueMakeStringDup(interp, ret, source_str);
        else {
            int slen;
            slen = (ce-source_str);
            if (slen)
                Jsi_DSAppendLen(&dStr, source_str, slen);
            if (replace_str)
                Jsi_DSAppendLen(&dStr, replace_str, replace_len);
            else {
                Jsi_Value *inStr = Jsi_ValueNewStringDup(interp, source_str);
                Jsi_IncrRefCount(interp, inStr);
                Jsi_RC rc = Jsi_FunctionInvokeString(interp, repVal, inStr, &dStr);
                if (Jsi_InterpGone(interp))
                    return JSI_ERROR;
                if (rc != JSI_OK) {
                    Jsi_DSFree(&dStr);
                    Jsi_DecrRefCount(interp, inStr);
                    return JSI_ERROR;
                }
                Jsi_DecrRefCount(interp, inStr);
            }
            Jsi_DSAppend(&dStr, ce+Jsi_Strlen(cp), NULL);
            Jsi_ValueFromDS(interp, &dStr, ret);
        }
        return JSI_OK;
    }
    if (seq == NULL || seq->vt != JSI_VT_OBJECT || seq->d.obj->ot != JSI_OT_REGEXP) {
        Jsi_ValueMakeNull(interp, ret);
        return JSI_OK;
    }

    re = seq->d.obj->d.robj;
    regex = &re->reg;
    isglob = (re->eflags & JSI_REG_GLOB);
    
    regmatch_t pmatch[MAX_SUBREGEX] = {};
    /* If an offset has been specified, adjust for that now.
     * If it points past the end of the string, point to the terminating null
     */
    int eoffset=0;
    if (offset) {
        if (offset < 0) {
            offset += source_len + 1;
        }
        if (offset > source_len) {
            offset = source_len;
        }
        else if (offset < 0) {
            offset = 0;
        }
    }

    Jsi_DSAppendLen(&dStr, source_str, offset);
    n = source_len - offset;
    p = source_str + offset;
    Jsi_RC rc = JSI_OK;
    do {
        if (num_matches > 10000000) {
            Jsi_LogBug("regexp infinite loop");
            rc = JSI_ERROR;
            break;
        }
        int match = regexec(regex, p, MAX_SUBREGEX, pmatch, regexec_flags);

        if (match >= REG_BADPAT) {
            char buf[JSI_MAX_NUMBER_STRING];

            regerror(match, regex, buf, sizeof(buf));
            Jsi_LogError("error while matching pattern: %s", buf);
            Jsi_DSFree(&dStr);
            return JSI_ERROR;
        }
        if (match == REG_NOMATCH) {
            break;
        }
        num_matches++;
        Jsi_DSAppendLen(&dStr, p, pmatch[0].rm_so);

        if (replace_str &&  !Jsi_Strchr(replace_str, '$'))
            Jsi_DSAppend(&dStr, replace_str, NULL);
        else if (replace_str) {
            for (j = 0; j < replace_len; j++) {
                int idx;
                int c = replace_str[j];
     
                if (c == '$' && j < replace_len) {
                    c = replace_str[++j];
                    if ((c >= '0') && (c <= '9')) {
                        idx = c - '0';
                    } else if (c == '&') {
                        idx = 0;
                    } else if (c == '$') {
                        Jsi_DSAppendLen(&dStr, replace_str + j, 1);
                        continue;
                    }
                    else if (c == '\'') {
                        Jsi_DSAppendLen(&dStr, p + pmatch[0].rm_eo, pmatch[0].rm_eo-Jsi_Strlen(p));
                        continue;
                    }
                    else if (c == '`') {
                        Jsi_DSAppendLen(&dStr, p, pmatch[0].rm_so);
                        continue;
                    }
                    else {
                        Jsi_DSAppendLen(&dStr, replace_str + j - 1, 2);
                        continue;
                    }
                } else {
                     Jsi_DSAppendLen(&dStr, replace_str + j, 1);
                     continue;
                }
                if ((idx < MAX_SUBREGEX) && pmatch[idx].rm_so != -1 && pmatch[idx].rm_eo != -1) {
                    Jsi_DSAppendLen(&dStr, p + pmatch[idx].rm_so,
                        pmatch[idx].rm_eo - pmatch[idx].rm_so);
                }
            }
    
        } else {
            Jsi_DString sStr;
            Jsi_DSInit(&sStr); 
            if (pmatch[0].rm_so <= 0 && pmatch[0].rm_eo <= 0)
                break;
            int olen = -1;
            char *ostr = jsi_SubstrDup(p, -1, pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, &olen);
            Jsi_Value *inStr = Jsi_ValueMakeBlob(interp, NULL, (uchar*)ostr, olen);
            Jsi_DSFree(&sStr);
            Jsi_IncrRefCount(interp, inStr);
            if (maxArgs==1) {
                Jsi_RC rc = Jsi_FunctionInvokeString(interp, repVal, inStr, &dStr);
                if (Jsi_InterpGone(interp))
                    return JSI_ERROR;
                if (rc != JSI_OK) {
                    Jsi_DSFree(&dStr);
                    Jsi_DecrRefCount(interp, inStr);
                    return JSI_ERROR;
                }
            } else {
                Jsi_Value *vpargs, *items[MAX_SUBREGEX] = {}, *ret;
                int i;
                items[0] = inStr;
                for (i=1; i<=(int)re->reg.re_nsub && i<(MAX_SUBREGEX-3); i++) {
                    if (pmatch[i].rm_so<0)
                        items[i] = interp->NullValue;
                    else {
                        ostr = jsi_SubstrDup(p, -1, pmatch[i].rm_so, pmatch[i].rm_eo - pmatch[i].rm_so, &olen);
                        items[i] = Jsi_ValueMakeBlob(interp, NULL, (uchar*)ostr, olen);
                    }
                }
                items[i++] = Jsi_ValueMakeNumber(interp, NULL, eoffset+pmatch[0].rm_so);
                items[i++] = strVal;
                vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, i, 0));
                Jsi_IncrRefCount(interp, vpargs);
                ret = Jsi_ValueNew1(interp);
                rc = Jsi_FunctionInvoke(interp, repVal, vpargs, &ret, NULL);
                if (Jsi_InterpGone(interp))
                    return JSI_ERROR;
                Jsi_DecrRefCount(interp, vpargs);
                if (rc == JSI_OK)
                    Jsi_DSAppend(&dStr, Jsi_ValueToString(interp, ret, NULL), NULL);
                Jsi_DecrRefCount(interp, ret);
                if (rc != JSI_OK) {
                    Jsi_DSFree(&dStr);
                    Jsi_DecrRefCount(interp, inStr);
                    return JSI_ERROR;
                }
            }
            Jsi_DecrRefCount(interp, inStr);
        }
        eoffset += pmatch[0].rm_eo;
        p += pmatch[0].rm_eo;
        n -= pmatch[0].rm_eo;
        /* If -all is not specified, or there is no source left, we are done */
        if (!isglob || n == 0 || pmatch[0].rm_eo == 0) {
            break;
        }
        /* An anchored pattern without -line must be done */
        if ((re->eflags & JSI_REG_NEWLINE) == 0 && re->pattern[0] == '^') {
            break;
        }
        
        /* If the pattern is empty, need to step forwards */
        if (re->pattern[0] == '\0' && n) {
            /* Need to copy the char we are moving over */
            Jsi_DSAppendLen(&dStr, p, 1);
            p++;
            n--;
        }

        regexec_flags |= REG_NOTBOL;
    } while (n);

    /*
     * Copy the portion of the string after the last match to the
     * result variable.
     */
    Jsi_DSAppend(&dStr, p, NULL);
    Jsi_ValueFromDS(interp, &dStr, ret);
    return rc;

}
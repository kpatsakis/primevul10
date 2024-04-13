static Jsi_RC jsiValueGetString(Jsi_Interp *interp, Jsi_Value* v, Jsi_DString *dStr, objwalker *owPtr)
{
    char buf[JSI_MAX_NUMBER_STRING], *str;
    Jsi_DString eStr;
    Jsi_DSInit(&eStr);
    if (interp->maxDepth>0 && owPtr->depth > interp->maxDepth)
        return Jsi_LogError("recursive ToString");
    int quote = owPtr->quote;
    int isjson = owPtr->quote&JSI_OUTPUT_JSON;
    Jsi_Number num;
    switch(v->vt) {
        case JSI_VT_UNDEF:
            Jsi_DSAppend(dStr, "undefined", NULL);
            return JSI_OK;
        case JSI_VT_NULL:
            Jsi_DSAppend(dStr, "null", NULL);
            return JSI_OK;
        case JSI_VT_VARIABLE:
            Jsi_DSAppend(dStr, "variable", NULL);
            return JSI_OK;
        case JSI_VT_BOOL:
            Jsi_DSAppend(dStr, (v->d.val ? "true":"false"), NULL);
            return JSI_OK;
        case JSI_VT_NUMBER:
            num = v->d.num;
outnum:
            if (isjson && !Jsi_NumberIsNormal(num)) {
                Jsi_DSAppend(dStr, "null", NULL);
            } else if (Jsi_NumberIsInteger(num)) {
                Jsi_NumberItoA10((Jsi_Wide)num, buf, sizeof(buf));
                Jsi_DSAppend(dStr, buf, NULL);
            } else if (Jsi_NumberIsWide(num)) {
                snprintf(buf, sizeof(buf), "%" PRId64, (Jsi_Wide)num);
                Jsi_DSAppend(dStr, buf, NULL);
            } else if (Jsi_NumberIsNormal(num) || Jsi_NumberIsSubnormal(num)) {
                Jsi_NumberDtoA(interp, num, buf, sizeof(buf), 0);
                Jsi_DSAppend(dStr, buf, NULL);
            } else if (Jsi_NumberIsNaN(num)) {
                Jsi_DSAppend(dStr, "NaN", NULL);
            } else {
                int s = Jsi_NumberIsInfinity(num);
                if (s > 0) Jsi_DSAppend(dStr, "+Infinity", NULL);
                else if (s < 0) Jsi_DSAppend(dStr, "-Infinity", NULL);
                else Jsi_LogBug("Ieee function problem: %d", fpclassify(num));
            }
            return JSI_OK;
        case JSI_VT_STRING:
            str = v->d.s.str;
outstr:
            if (!quote) {
                Jsi_DSAppend(dStr, str, NULL);
                return JSI_OK;
            }
            Jsi_DSAppend(dStr,"\"", NULL);
            while (*str) {
                if ((*str == '\'' && (!isjson)) || *str == '\\'|| *str == '\"'|| (*str == '\n'
                    && (!(owPtr->quote&JSI_OUTPUT_NEWLINES)))
                    || *str == '\r' || *str == '\t' || *str == '\f' || *str == '\b'  ) {
                    char pcp[2];
                    *pcp = *str;
                    pcp[1] = 0;
                    Jsi_DSAppendLen(dStr,"\\", 1);
                    switch (*str) {
                        case '\r': *pcp = 'r'; break;
                        case '\n': *pcp = 'n'; break;
                        case '\t': *pcp = 't'; break;
                        case '\f': *pcp = 'f'; break;
                        case '\b': *pcp = 'b'; break;
                    }
                    Jsi_DSAppendLen(dStr,pcp, 1);
                } else if (isprint(*str) || !isjson)
                    Jsi_DSAppendLen(dStr,str, 1);
                else {
                    char ubuf[10];
                    int l = Jsi_UtfEncode(str, ubuf);
                    Jsi_DSAppend(dStr,ubuf, NULL);
                    str += l-1;
                }
                str++;
            }
            Jsi_DSAppend(dStr,"\"", NULL);
            Jsi_DSFree(&eStr);
            return JSI_OK;
        case JSI_VT_OBJECT: {
            Jsi_Obj *o = v->d.obj;
            switch(o->ot) {
                case JSI_OT_BOOL:
                    Jsi_DSAppend(dStr, (o->d.val ? "true":"false"), NULL);
                    return JSI_OK;
                case JSI_OT_NUMBER:
                    num = o->d.num;
                    goto outnum;
                    return JSI_OK;
                case JSI_OT_STRING:
                    str = o->d.s.str;
                    goto outstr;
                case JSI_OT_FUNCTION:
                    Jsi_FuncObjToString(interp, o->d.fobj->func, &eStr, 3 | ((owPtr->depth==0 && owPtr->quote)?8:0));
                    str = Jsi_DSValue(&eStr);
                    goto outstr;
                case JSI_OT_REGEXP:
                    str = o->d.robj->pattern;
                    goto outstr;
                case JSI_OT_USEROBJ:
                    jsi_UserObjToName(interp, o->d.uobj, &eStr);
                    str = Jsi_DSValue(&eStr);
                    goto outstr;
                case JSI_OT_ITER:
                    Jsi_DSAppend(dStr, (isjson?"null":"*ITER*"), NULL);
                    return JSI_OK;
                default:
                    break;
            }
                        
            if (o->isarrlist)
            {
                Jsi_Value *nv;
                int i, len = o->arrCnt;
                
                if (!o->arr)
                    len = Jsi_ValueGetLength(interp, v);
                Jsi_DSAppend(dStr,"[",len?" ":"", NULL);
                for (i = 0; i < len; ++i) {
                    nv = Jsi_ValueArrayIndex(interp, v, i);
                    if (i) Jsi_DSAppend(dStr,", ", NULL);
                    owPtr->depth++;
                    if (nv) {
                        if (jsiValueGetString(interp, nv, dStr, owPtr) != JSI_OK) {
                            owPtr->depth--;
                            return JSI_ERROR;
                        }
                    }
                    else Jsi_DSAppend(dStr, "undefined", NULL);
                    owPtr->depth--;
                }
                Jsi_DSAppend(dStr,len?" ":"","]", NULL);
            } else {
                int len = Jsi_TreeSize(o->tree);
                Jsi_DSAppend(dStr,"{",len?" ":"", NULL);
                owPtr->depth++;
                Jsi_TreeWalk(o->tree, _object_get_callback, owPtr, 0);
                owPtr->depth--;
                Jsi_DSAppend(dStr,len?" ":"","}", NULL);
            }
            return JSI_OK;
        }
#ifndef __cplusplus
        default:
            Jsi_LogBug("Unexpected value type: %d", v->vt);
#endif
    }
    return JSI_OK;
}
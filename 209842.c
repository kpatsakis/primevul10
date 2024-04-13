static Jsi_RC StringMapCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    /* Now handles perl regex flag extensions.*/
    const char *source_str;
    const char *replace_str = NULL;
    uint i, j;
    int slen, source_len, replace_len, bLen;

    const char *p;
    bool nocase = 0;
    /* Is a generic  String.replace if _this->d.obj is a function */
    ChkString(_this, funcPtr, source_str, &source_len, &bLen);
    int argc = Jsi_ValueGetLength(interp, args);
    Jsi_Value *repVal = Jsi_ValueArrayIndex(interp, args, skip);
    if (Jsi_ValueIsArray(interp, repVal)==0 || repVal->d.obj->arrCnt&1) 
        return Jsi_LogError("expected even length array");
    if (argc>(skip+1) && Jsi_ValueGetBoolean(interp, Jsi_ValueArrayIndex(interp, args, skip+1), &nocase) != JSI_OK) 
        return Jsi_LogError("expected boolean");
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_Obj *obj = repVal->d.obj;
    p = source_str;
    for (j=0; j<(uint)bLen; j++, p++) {
        for (i=0; i<obj->arrCnt; i+=2) {
            if (!obj->arr[i]) continue;
            if (!obj->arr[i+1]) continue;
            const char *cp = Jsi_ValueToString(interp, obj->arr[i], &slen);
            if (!cp || !slen) {
                Jsi_DSFree(&dStr);
                return Jsi_LogError("map src can not be empty");
            }
            int res = (nocase ? Jsi_Strncasecmp(cp, p, slen) : Jsi_Strncmp(cp, p, slen));
            if (!res) {
                replace_str = Jsi_ValueToString(interp, obj->arr[i+1], &replace_len);
                Jsi_DSAppendLen(&dStr, replace_str, replace_len);
                p += slen-1;
                j += slen-1;
                break;
            }
        }
        if (i>=obj->arrCnt)
            Jsi_DSAppendLen(&dStr, p, 1);
    }

    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
}
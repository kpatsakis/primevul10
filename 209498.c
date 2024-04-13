static Jsi_RC StringSliceCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *vstr;
    ChkString(_this, funcPtr, vstr, &sLen, &bLen);

    Jsi_Value *start = Jsi_ValueArrayIndex(interp, args, skip);
    Jsi_Value *end = Jsi_ValueArrayIndex(interp, args, 1+skip);
    int istart, iend, len = Jsi_Strlen(vstr);

    if (Jsi_GetIntFromValue(interp, start, &istart)) {
        return JSI_ERROR;
    }
    if (istart < 0)
        istart = len+istart;
    if (istart < 0)
        istart = 0;
    if (istart>=len)
        istart = len-1;
    iend = len;
    if (end) {
        if (Jsi_GetIntFromValue(interp, end, &iend)) {
            return JSI_ERROR;
        }
        if (iend < 0)
            iend = len+iend;
        if (iend>=len)
            iend = len;
        if (iend<istart)
            iend = istart;
    }
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_UtfSubstr(vstr, istart, iend, &dStr);
    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
}
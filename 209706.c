static Jsi_RC StringSubstringCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen = 0, bLen;
    const char *v;
    ChkString(_this, funcPtr, v, &sLen, &bLen);

    Jsi_Value *start = Jsi_ValueArrayIndex(interp, args, skip);
    Jsi_Value *end = Jsi_ValueArrayIndex(interp, args, skip+1);
    
    Jsi_Number nstart, nend;
    if (!start || Jsi_GetNumberFromValue(interp,start, &nstart) != JSI_OK) {
        Jsi_ValueMakeStringDup(interp, ret, v);
        return JSI_OK;
    }
    int istart = (int)nstart, olen = -1;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    char *ostr;
    if (!end || Jsi_GetNumberFromValue(interp,end, &nend) != JSI_OK) {
        if (sLen == bLen) {
            ostr = jsi_SubstrDup(v, bLen, istart, -1, &olen);
            Jsi_ValueMakeBlob(interp, ret, (uchar*)ostr, olen);
        } else {
            Jsi_UtfSubstr(v, istart, -1, &dStr);
            Jsi_ValueFromDS(interp, &dStr, ret);
        }
        return JSI_OK;
    }
    int iend = (int)nend;
    if (iend>sLen)
        iend = sLen;
    if (iend < istart) {
        Jsi_ValueMakeStringDup(interp, ret, "");
    } else {
        if (sLen == bLen) {
            ostr = jsi_SubstrDup(v, bLen, istart, iend-istart+1, &olen);
            Jsi_ValueMakeBlob(interp, ret, (uchar*)ostr, olen);
        } else {
            Jsi_UtfSubstr(v, istart, iend-istart+1, &dStr);
            Jsi_ValueFromDS(interp, &dStr, ret);
        }
    }
    return JSI_OK;
}
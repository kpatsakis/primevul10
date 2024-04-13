static Jsi_RC StringSubstrCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *v;
    ChkString(_this, funcPtr, v, &sLen, &bLen);

    Jsi_Value *start = Jsi_ValueArrayIndex(interp, args, skip);
    Jsi_Value *len = Jsi_ValueArrayIndex(interp, args, skip+1);
    
    Jsi_Number nstart, nlen;
    if (!start || Jsi_GetNumberFromValue(interp,start, &nstart) != JSI_OK) {
        jsi_ValueMakeBlobDup(interp, ret, (uchar*)v, bLen);
        return JSI_OK;
    }
    int istart = (int)nstart, olen = -1;
    Jsi_DString dStr;
    char *ostr;
    Jsi_DSInit(&dStr);
    if (!len || Jsi_GetNumberFromValue(interp,len, &nlen) != JSI_OK) {
        if (sLen == bLen) {
            ostr = jsi_SubstrDup(v, bLen, istart, -1, &olen);
            Jsi_ValueMakeBlob(interp, ret, (uchar*)ostr, olen);
        } else {
            Jsi_UtfSubstr(v, istart, -1, &dStr);
            Jsi_ValueFromDS(interp, &dStr, ret);
        }
        return JSI_OK;
    }
    int ilen = (int)nlen;
    if (ilen <= 0) {
        Jsi_ValueMakeStringDup(interp, ret, "");
    } else {
        if (sLen == bLen) {
            ostr = jsi_SubstrDup(v, bLen, istart, ilen, &olen);
            Jsi_ValueMakeBlob(interp, ret, (uchar*)ostr, olen);
        } else {
            Jsi_UtfSubstr(v, istart, ilen, &dStr);
            Jsi_ValueFromDS(interp, &dStr, ret);
        }
    }
    return JSI_OK;
}
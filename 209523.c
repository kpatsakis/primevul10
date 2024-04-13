static Jsi_RC StringCharCodeAtCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *source_str;
    ChkString(_this, funcPtr, source_str, &sLen, &bLen);
    const char *v = source_str;

    Jsi_Value *ttPtr = Jsi_ValueNew1(interp);
    Jsi_ValueCopy(interp, ttPtr, _this);
    
    int pos = 0;
    Jsi_Value *vpos;
    if ((vpos = Jsi_ValueArrayIndex(interp, args, skip))) {
        jsi_ValueToOInt32(interp, vpos);
        pos = (int)vpos->d.num;
    }

#if JSI__UTF8
    int m;
    if (pos >= 0 && pos < sLen && ((m=Jsi_UtfIndexToOffset(v, pos))>=0)) {
        int32_t n;
        Jsi_UtfToUniChar(v+m, &n);
        Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)n);
    }
#else
    int slen = Jsi_Strlen(v);
    if (pos >= 0 && pos < slen) {
        Jsi_ValueMakeNumber(interp, ret, v[pos]);
    }
#endif
    else
        Jsi_ValueMakeNumber(interp, ret, Jsi_NumberNaN());
    Jsi_DecrRefCount(interp, ttPtr);
    return JSI_OK;
}
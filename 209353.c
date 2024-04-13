static Jsi_RC StringRepeatCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *v;
    ChkString(_this, funcPtr, v, &sLen, &bLen);
    
    Jsi_Value *start = Jsi_ValueArrayIndex(interp, args, skip);
    
    Jsi_Number ncnt;
    if (!start || Jsi_GetNumberFromValue(interp,start, &ncnt) != JSI_OK || ncnt>MAX_LOOP_COUNT || ncnt<0 || bLen<=0) {
        return JSI_ERROR;
    }
    int cnt = (int)ncnt;
    Jsi_DString dStr = {};
    while (cnt-- > 0) {
        Jsi_DSAppendLen(&dStr, v, bLen);
        if (Jsi_DSLength(&dStr)>JSI_MAX_ALLOC_BUF) {
            Jsi_DSFree(&dStr);
            return Jsi_LogError("too long");
        }
    }
    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
}
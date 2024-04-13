static Jsi_RC StringCharAtCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int pos = 0, len, bLen;
    const char *vstr;
    ChkString(_this, funcPtr, vstr, &len, &bLen);
   
    Jsi_Value *vpos = Jsi_ValueArrayIndex(interp, args, 0);
    if (Jsi_GetIntFromValue(interp, vpos, &pos)) {
        return JSI_ERROR;        
    }
    if (pos<0 || pos >=len)
        Jsi_ValueMakeStringDup(interp, ret, "");
    else {
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        Jsi_UtfSubstr(vstr, pos, 1, &dStr);
        Jsi_ValueFromDS(interp, &dStr, ret);
    }
    return JSI_OK;
}
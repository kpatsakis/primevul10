static Jsi_RC StringToTitleCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *vstr;
    ChkString(_this, funcPtr, vstr, &sLen, &bLen);
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    jsi_utf_tocase(vstr, 2, &dStr);
    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
}
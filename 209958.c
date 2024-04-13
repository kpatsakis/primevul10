static Jsi_RC StringConcatCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *vstr;
    ChkString(_this, funcPtr, vstr, &sLen, &bLen);
    
    Jsi_DString dStr = {};
    Jsi_DSAppend(&dStr, vstr, NULL);
    int i, argc = Jsi_ValueGetLength(interp, args);
    for (i=skip; i<argc; i++)
    {
        Jsi_Value *s = Jsi_ValueArrayIndex(interp, args, i);
        if (Jsi_GetStringFromValue(interp, s, &vstr)) {
            Jsi_LogError("String get failure");
            Jsi_DSFree(&dStr);
            return JSI_ERROR;
        }
        Jsi_DSAppend(&dStr, vstr, NULL);
    }

    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
    
}
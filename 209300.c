static Jsi_RC RegexpTestCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int skip = 0, rc = 0;
    Jsi_Value *v;
    ChkRegexp(_this, funcPtr, v);
    char *str = Jsi_ValueString(interp,Jsi_ValueArrayIndex(interp, args, skip), NULL);
    if (!str) 
        return Jsi_LogError("expected string");
    if (Jsi_RegExpMatch(interp, v, str, &rc, NULL) != JSI_OK)
        return JSI_ERROR;    
    Jsi_ValueMakeBool(interp, ret, rc != 0);
    return JSI_OK;
}
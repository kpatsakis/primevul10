static Jsi_RC JSONParseCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    bool strict = 1;
    Jsi_Value *jsv = Jsi_ValueArrayIndex(interp, args, 1);
    if (jsv && Jsi_ValueGetBoolean(interp, jsv, &strict) != JSI_OK) 
        return Jsi_LogError("Expected boolean");
    jsv = Jsi_ValueArrayIndex(interp, args, 0); 
    const char *js = Jsi_ValueToString(interp, jsv, NULL);
    return Jsi_JSONParse(interp, js, ret, strict);
}
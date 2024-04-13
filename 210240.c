static Jsi_RC JSONCheckCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{

    int r;
    Jsi_RC result = JSI_OK;
    bool strict = 1;
    const char *cp;
    JSI_JSON_DECLARE(p, tokens, 0);
    Jsi_Value *jsv = Jsi_ValueArrayIndex(interp, args, 1);
    if (jsv && Jsi_ValueGetBoolean(interp, jsv, &strict) != JSI_OK) 
        return Jsi_LogError("Expected boolean");
    p.strict = strict;
    jsv = Jsi_ValueArrayIndex(interp, args, 0);
    cp = Jsi_ValueToString(interp, jsv, NULL);
    
    r = Jsi_JsonParse(&p, cp);
    Jsi_ValueMakeBool(interp, ret, r == JSI_JSON_ERR_NONE);
    Jsi_JsonFree(&p);
    return result;
}
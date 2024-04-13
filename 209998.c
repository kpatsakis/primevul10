static Jsi_RC StringMatchCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *source_str;
    ChkString(_this, funcPtr, source_str, &sLen, &bLen);
    const char *v = source_str;
    Jsi_Value *seq = Jsi_ValueArrayIndex(interp, args, skip);

    if (Jsi_ValueIsString(interp, seq)) {
        char *cp = Jsi_ValueString(interp, seq, NULL);

        if (jsi_RegExpValueNew(interp, cp, seq) != JSI_OK)
            return JSI_ERROR;
    }
    /* TODO: differentiate from re.exec() */
    return jsi_RegExpMatches(interp, seq, v, bLen, *ret, NULL, 1);
}
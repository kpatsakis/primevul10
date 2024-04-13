Jsi_RC Jsi_RegExpMatches(Jsi_Interp *interp, Jsi_Value *pattern, const char *str, int slen, Jsi_Value *ret)
{
    return jsi_RegExpMatches(interp, pattern, str, slen, ret, NULL, 0);
}
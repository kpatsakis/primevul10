jsi_SetOption(Jsi_Interp *interp, Jsi_OptionSpec *specPtr, const char *string /*UNUSED*/, void* rec, Jsi_Value *argValue, Jsi_Wide flags, bool isSafe)
{
    Jsi_Value *oa = interp->lastParseOpt;
    interp->lastParseOpt = argValue;
    Jsi_RC rc = jsi_SetOption_(interp, specPtr, string, rec, argValue, flags, isSafe);
    interp->lastParseOpt = oa;
    return rc;
}
Jsi_RC Jsi_EvalString(Jsi_Interp* interp, const char *str, int flags)
{
    int lev = interp->framePtr->level;
    return jsi_evalStrFile(interp, NULL, (char*)str, flags, lev);
}
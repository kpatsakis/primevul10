Jsi_RC Jsi_EvalFile(Jsi_Interp* interp, Jsi_Value *fname, int flags)
{
    int isnull;
    if ((isnull=Jsi_ValueIsNull(interp, fname)) || Jsi_ValueIsUndef(interp, fname)) 
        return Jsi_LogError("invalid file eval %s", (isnull?"null":"undefined"));
    int lev = interp->framePtr->level;
    return jsi_evalStrFile(interp, fname, NULL, flags, lev);
}
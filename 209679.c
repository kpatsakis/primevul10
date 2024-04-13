const char *jsi_PstateGetFilename(jsi_Pstate *ps)
{
    Jsi_Interp *interp = ps->interp;
    return interp->curFile;
}
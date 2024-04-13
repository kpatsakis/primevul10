static bool jsi_LogEnabled(Jsi_Interp *interp, uint code) {
    if (!interp->activeFunc) return 0;
    Jsi_CmdSpec *cs = interp->activeFunc->cmdSpec;
    if (!cs)
        return 0;
    if (interp->activeFunc->parentSpec)
        cs = interp->activeFunc->parentSpec;
    int cofs = (code - JSI_LOG_TEST);
    int ac = (cs->flags & (JSI_CMD_LOG_TEST<<cofs));
    return (ac)!=0;
}
Jsi_RC jsi_InitRegexp(Jsi_Interp *interp, int release)
{
    if (!release)
        interp->RegExp_prototype = Jsi_CommandCreateSpecs(interp, "RegExp", regexpCmds, NULL, JSI_CMDSPEC_ISOBJ);
    return JSI_OK;
}
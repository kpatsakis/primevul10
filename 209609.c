Jsi_RC jsi_InitString(Jsi_Interp *interp, int release)
{
    if (!release)
        interp->String_prototype = Jsi_CommandCreateSpecs(interp, "String", stringCmds, NULL, JSI_CMDSPEC_ISOBJ);
    return JSI_OK;
}
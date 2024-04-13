Jsi_RC jsi_InitJSON(Jsi_Interp *interp, int release) {
    if (release) return JSI_OK;
    Jsi_CommandCreateSpecs(interp, "JSON", jsonCmds, NULL, 0);
#ifdef TEST_JSON_INVOKE
    Jsi_Value *ret = Jsi_ValueNew1(interp);
    Jsi_CommandInvokeJSON(interp, "Info.cmds", "[\"*\", true]", ret);
    Jsi_DString dStr = {};
    Jsi_Puts(NULL, Jsi_ValueGetDString(interp, ret, &dStr, 1), -1);
    Jsi_DSFree(&dStr);
    Jsi_DecrRefCount(interp, ret);
#endif
    return JSI_OK;
}
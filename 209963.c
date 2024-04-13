Jsi_RC Jsi_CommandInvokeJSON(Jsi_Interp *interp, const char *cmdstr, const char *json, Jsi_Value **ret)
{
    Jsi_Value *func = Jsi_NameLookup(interp, cmdstr);
    if (func)
        return Jsi_FunctionInvokeJSON(interp, func, json, ret);
    return Jsi_LogError("can not find cmd: %s", cmdstr);
}
Jsi_RC Jsi_CommandInvoke(Jsi_Interp *interp, const char *cmdstr, Jsi_Value *args, Jsi_Value **ret)
{
    Jsi_Value *func = Jsi_NameLookup(interp, cmdstr);
    if (func)
        return Jsi_FunctionInvoke(interp, func, args, ret, NULL);
    return Jsi_LogError("can not find cmd: %s", cmdstr);
}
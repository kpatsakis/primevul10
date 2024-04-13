Jsi_RC Jsi_FunctionInvokeJSON(Jsi_Interp *interp, Jsi_Value *func, const char *json, Jsi_Value **ret)
{
    if (!Jsi_ValueIsFunction(interp, func))
        return JSI_ERROR;
    Jsi_Value *aPtr = Jsi_ValueNew1(interp);
    Jsi_RC rc = Jsi_JSONParse(interp, json, &aPtr, 0);
    if (rc == JSI_OK)
        rc = Jsi_FunctionInvoke(interp, func, aPtr, ret, NULL);
    Jsi_DecrRefCount(interp, aPtr);
    return rc;
}
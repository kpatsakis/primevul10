static Jsi_RC InterpInfoCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    InterpObj *udf = (InterpObj *)Jsi_UserObjGetData(interp, _this, funcPtr);
    Jsi_Interp *subinterp = interp;
    if (udf) {
        if (!udf->subinterp)
            return Jsi_LogError("Sub-interp gone");
        subinterp = udf->subinterp;
    }
    return jsi_InterpInfo(subinterp, args, _this, ret, funcPtr);
}
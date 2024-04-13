static Jsi_RC FilesysTruncateCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    Jsi_Value *vPos = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_Number num;
    if (Jsi_ValueGetNumber(interp, vPos, &num) != JSI_OK)
        return JSI_ERROR;
    num = (Jsi_Number)Jsi_Truncate(interp, udf->chan, (unsigned int)num);
    Jsi_ValueMakeNumber(interp, ret, num);
    return JSI_OK;
}
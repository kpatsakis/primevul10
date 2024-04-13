static Jsi_RC FilesysModeCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    if (udf->mode)
        Jsi_ValueMakeStringKey(interp, ret, udf->mode);
    return JSI_OK;
}
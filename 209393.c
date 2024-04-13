static Jsi_RC FilesysTellCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    Jsi_Wide pos = Jsi_Tell(interp, udf->chan);
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)pos);
    return JSI_OK;
}
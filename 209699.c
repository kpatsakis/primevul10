static Jsi_RC FilesysEofCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    Jsi_ValueMakeBool(interp, ret, Jsi_Eof(interp, udf->chan));
    return JSI_OK;
}
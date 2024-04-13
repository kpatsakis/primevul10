static Jsi_RC FilesysFilenameCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    if (udf->filename)
        Jsi_ValueMakeStringDup(interp, ret, udf->filename);
    return JSI_OK;
}
static Jsi_RC FilesysCloseCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    fileObjErase(udf);
    Jsi_ValueMakeBool(interp, ret, 1);
    return JSI_OK;
}
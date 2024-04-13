static Jsi_RC FilesysOpenCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
    if (try_open_file(interp, udf, args) != JSI_OK) {
        Jsi_ValueMakeBool(interp, ret, 0);
    }
    Jsi_ValueMakeBool(interp, ret, 1);
    return JSI_OK;
}
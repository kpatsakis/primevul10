static Jsi_RC FilesysStatCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdfGet(udf, _this, funcPtr);
#if JSI__FILESYS==1
    return jsi_FileStatCmd(interp, udf->fname, _this, ret, funcPtr, 0);
#else
    return JSI_ERROR;
#endif
}
static Jsi_RC FilesysGetsCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int len;
    UdfGet(udf, _this, funcPtr);
    if (!udf->filename) {
        Jsi_ValueMakeUndef(interp, ret);
        return JSI_OK;
    }
    char buf[JSI_BUFSIZ];
    if (!Jsi_Gets(interp, udf->chan, buf, sizeof(buf))) {
        Jsi_ValueMakeUndef(interp, ret);
        return JSI_OK;
    }
    buf[sizeof(buf)-1] = 0;
    len = Jsi_Strlen(buf);
    if (len > 0 && buf[len-1] == '\n')
        buf[len-1] = 0;
    Jsi_ValueMakeStringDup(interp, ret, buf);
    return JSI_OK;
}
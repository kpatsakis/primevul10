static Jsi_RC FilesysSeekCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    static const char *posStr[] = { "set", "cur", "end", NULL };
    enum { W_SET, W_CUR, W_END };
    UdfGet(udf, _this, funcPtr);
    Jsi_Value *vPos = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_Value *vWhence = Jsi_ValueArrayIndex(interp, args, 1);
    int mode = 0, p;
    Jsi_Wide pos;
    Jsi_Number num;
    if (Jsi_ValueGetNumber(interp, vPos, &num) != JSI_OK)
        return JSI_ERROR;
    if (Jsi_ValueGetIndex(interp, vWhence, posStr, "position", 0, &p) != JSI_OK)
        return JSI_ERROR;
    switch (p) {
        case W_SET: mode = SEEK_SET; break;
        case W_CUR: mode = SEEK_CUR; break;
        case W_END: mode = SEEK_END; break;
    }
    pos = (Jsi_Wide)num;
    pos = Jsi_Seek(interp, udf->chan, pos, mode);
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)pos);
    return JSI_OK;
}
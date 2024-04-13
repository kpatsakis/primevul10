static Jsi_RC FilesysPutsCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
   
    UdfGet(udf, _this, funcPtr);
    if (!udf->filename) {
        Jsi_ValueMakeBool(interp, ret, 0);
        return JSI_OK;
    }
    Jsi_Value *toput = Jsi_ValueArrayIndex(interp, args, 0);
    if (!toput) {
        Jsi_ValueMakeBool(interp, ret, 0);
        return JSI_OK;
    }
    const char * cp = Jsi_ValueToString(interp, toput, NULL);

    if (Jsi_Printf(interp, udf->chan, "%s\n", cp?cp:"") < 0) {
        Jsi_ValueMakeBool(interp, ret, 0);
        return JSI_OK;
    }
    Jsi_ValueMakeBool(interp, ret, 1);
    return JSI_OK;
}
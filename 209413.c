static Jsi_RC SqliteCompleteCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                             Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb;
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *s = Jsi_ValueArrayIndex(interp, args, 0);
    const char *str =  Jsi_ValueString(interp, s, NULL);
    int isComplete = 0;
    if (str)
        isComplete = sqlite3_complete( str );
    Jsi_ValueMakeBool(interp, ret, isComplete);
    return JSI_OK;
}
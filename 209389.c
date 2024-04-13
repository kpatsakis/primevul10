static Jsi_RC SqliteInterruptCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb;
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    sqlite3_interrupt(jdb->db);
    return JSI_OK;
}
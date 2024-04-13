static Jsi_RC SqliteFilenameCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                             Jsi_Value **ret, Jsi_Func *funcPtr)
{
#if (SQLITE_VERSION_NUMBER>3007009)
    const char *zName = "main";
    int argc = Jsi_ValueGetLength(interp, args);
    Jsi_Db *jdb;

    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    if (argc)
        zName = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    zName = sqlite3_db_filename(jdb->db, zName);
    if (zName)
        Jsi_ValueMakeStringDup(interp, ret, zName);
#endif
    return JSI_OK;
}
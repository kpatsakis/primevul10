static Jsi_Db *dbGetDbHandle(Jsi_Interp *interp, Jsi_Value *_this, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb = (Jsi_Db*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!jdb) {
        Jsi_LogError("Sqlite call to a non-sqlite object");
        return NULL;
    }
    if (!jdb->db)
    {
        Jsi_LogError("Sqlite db closed");
        return NULL;
    }
    return jdb;
}
static Jsi_RC SqliteCollateCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                            Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb;
    Jsi_Value *func;

    SqlCollate *pCollate;
    char *zName;
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;

    zName = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    func = Jsi_ValueArrayIndex(interp, args, 1);
    pCollate = (SqlCollate*)Jsi_Calloc(1, sizeof(*pCollate));
    if( pCollate==0 ) return JSI_ERROR;
    pCollate->jdb = jdb;
    pCollate->interp = interp;
    pCollate->pNext = jdb->pCollate;
    pCollate->zScript = func; /*(char*)&pCollate[1];*/
    jdb->pCollate = pCollate;

    if( sqlite3_create_collation(jdb->db, zName, SQLITE_UTF8, pCollate, dbSqlCollate) )
    
        return Jsi_LogError("%s", (char *)sqlite3_errmsg(jdb->db));
    return JSI_OK;
}
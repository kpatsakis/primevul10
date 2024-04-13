static Jsi_RC SqliteConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                         Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb, ojdb;
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *opts = Jsi_ValueArrayIndex(interp, args, 0);
    if (jdb->noConfig && opts && !Jsi_ValueIsString(interp, opts))
        return Jsi_LogError("Socket conf() is disabled for set");
    ojdb = *jdb;
    
    jdb->lastInsertId = sqlite3_last_insert_rowid(jdb->db);
    jdb->changeCnt = sqlite3_changes(jdb->db);
    jdb->changeCntAll = sqlite3_total_changes(jdb->db);
    jdb->errorCode = sqlite3_errcode(jdb->db);
    
    Jsi_RC rc = Jsi_OptionsConf(interp, SqlOptions, jdb, opts, ret, 0);
    
    if (jdb->stmtCacheMax<0 || jdb->stmtCacheMax>MAX_PREPARED_STMTS) {
        JSI_DBQUERY_PRINTF( "option stmtCacheMax value %d is not in range 0..%d", jdb->stmtCacheMax, MAX_PREPARED_STMTS);
        jdb->stmtCacheMax = ojdb.stmtCacheMax;
        rc = JSI_ERROR;
    }
    dbSetupCallbacks(jdb, &ojdb);
    dbPrepStmtLimit(jdb);
    return rc;
}
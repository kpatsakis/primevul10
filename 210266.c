static Jsi_RC SqliteRestoreCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                            Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Db *jdb;
    const char *zSrcFile;
    const char *zDestDb;
    sqlite3 *pSrc;
    sqlite3_backup *pBackup;
    int nTimeout = 0;
    int rc;

    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *vFile = Jsi_ValueArrayIndex(interp, args, 0);
    int argc = Jsi_ValueGetLength(interp, args);
    if( argc==1 ) {
        zDestDb = "main";
    } else {
        zDestDb = Jsi_ValueArrayIndexToStr(interp, args, 1, NULL);
    }
    Jsi_DString dStr = {};
    if (!vFile)
        zSrcFile = ":memory:";
    else {
        zSrcFile = Jsi_ValueNormalPath(interp, vFile, &dStr);
        if (zSrcFile == NULL) 
            return Jsi_LogError("bad or missing file name");
    }
    rc = sqlite3_open_v2(zSrcFile, &pSrc, SQLITE_OPEN_READONLY, 0);

    if( rc!=SQLITE_OK ) {
        Jsi_LogError("cannot open source database: %s", sqlite3_errmsg(pSrc));
        DbClose(pSrc);
        Jsi_DSFree(&dStr);
        return JSI_ERROR;
    }
    pBackup = sqlite3_backup_init(jdb->db, zDestDb, pSrc, "main");
    if( pBackup==0 ) {
        Jsi_LogError("restore failed: %s", sqlite3_errmsg(jdb->db));
        DbClose(pSrc);
        Jsi_DSFree(&dStr);
        return JSI_ERROR;
    }
    while( (rc = sqlite3_backup_step(pBackup,100))==SQLITE_OK
            || rc==SQLITE_BUSY ) {
        if( rc==SQLITE_BUSY ) {
            if( nTimeout++ >= 3 ) break;
            sqlite3_sleep(100);
        }
    }
    sqlite3_backup_finish(pBackup);
    Jsi_RC rv;
    if( rc==SQLITE_DONE ) {
        rv = JSI_OK;
    } else if( rc==SQLITE_BUSY || rc==SQLITE_LOCKED ) {
        Jsi_LogError("restore failed: source database busy");
        rv = JSI_ERROR;
    } else {
        Jsi_LogError("restore failed: %s", sqlite3_errmsg(jdb->db));
        rv = JSI_ERROR;
    }
    Jsi_DSFree(&dStr);
    DbClose(pSrc);
    return rv;
}
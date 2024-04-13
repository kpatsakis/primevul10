static Jsi_RC dbEvalStepSub(DbEvalContext *p, int release, int *erc) {
    int rcs;
    Jsi_Db *jdb = p->jdb;
    Jsi_Interp *interp = jdb->interp;
    JSI_NOTUSED(interp);
    SqlPreparedStmt *pPreStmt = p->pPreStmt;
    SQLSIGASSERT(pPreStmt, STMT);
    sqlite3_stmt *pStmt = pPreStmt->pStmt;

    if (jdb->debug & TMODE_STEP)
        JSI_DBQUERY_PRINTF( "DEBUG: step: %s\n", pPreStmt->zSql);
    rcs = sqlite3_step(pStmt);
    if( rcs==SQLITE_BUSY ) {
        if (erc) *erc = -2;
        return JSI_ERROR;
    }
    if( rcs==SQLITE_ROW ) {
        return JSI_OK;
    }
    if( p->pArray ) {
        dbEvalRowInfo(p, 0, 0, 0);
    }
    rcs = sqlite3_reset(pStmt);

    jdb->stepCnt = sqlite3_stmt_status(pStmt,SQLITE_STMTSTATUS_FULLSCAN_STEP,1);
    jdb->sortCnt = sqlite3_stmt_status(pStmt,SQLITE_STMTSTATUS_SORT,1);
    if (release==0 && rcs==SQLITE_OK)
        return JSI_BREAK;
    dbReleaseColumnNames(p);
    p->pPreStmt = 0;

    if( rcs!=SQLITE_OK ) {
        /* If a run-time error occurs, report the error and stop reading
        ** the SQL.  */
        Jsi_LogError("%s", sqlite3_errmsg(jdb->db));
        dbReleaseStmt(jdb, pPreStmt, 1);
        return JSI_ERROR;
    } else {
        dbReleaseStmt(jdb, pPreStmt, p->nocache);
    }
    return JSI_BREAK;
}
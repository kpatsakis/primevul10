static Jsi_RC SqliteEvalCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                         Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int rc = SQLITE_OK, rc2;
    Jsi_Db *jdb;
    sqlite3_stmt *pStmt = NULL;

    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    sqlite3 *db = jdb->db;
    const char *zSql = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    const char *zStart = zSql, *zLeftover = NULL, *zErrMsg = NULL;
    int lnum = 1;
    if (jdb->echo && zSql)
        Jsi_LogInfo("SQL-EVAL: %s\n", zSql); 

    while( zSql && zSql[0] && (SQLITE_OK == rc) ) {
        rc = sqlite3_prepare_v2(db, zSql, -1, &pStmt, &zLeftover);

        if( SQLITE_OK != rc ) {
            break;
        } else {
            if( !pStmt ) {
                /* this happens for a comment or white-space */
                zSql = zLeftover;
                while( isspace(zSql[0]) ) zSql++;
                continue;
            }

            do {
                if (jdb->debug & TMODE_STEP)
                    JSI_DBQUERY_PRINTF( "DEBUG: step: %s\n", zSql);
                rc = sqlite3_step(pStmt);
            } while( rc == SQLITE_ROW );
            rc2 = sqlite3_finalize(pStmt);
            if( rc!=SQLITE_NOMEM ) rc = rc2;
            if( rc==SQLITE_OK ) {
                zSql = zLeftover;
                while( isspace(zSql[0]) ) zSql++;
            } else {
            }
        }
    }
 
    if (rc == SQLITE_OK) {
        Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)sqlite3_changes(jdb->db));
        return JSI_OK;
    }
    while (zSql && zStart<zSql) {
        if (zStart[0] == '\n') lnum++;
        zStart++;
    }
    zErrMsg = sqlite3_errmsg(db);
    Jsi_LogError("sqlite error: %s in statement at line %d", (zErrMsg ? zErrMsg : ""), lnum);
    return JSI_ERROR;
}
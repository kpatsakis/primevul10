static Jsi_RC mdbEvalStepSub(MyDbEvalContext *eval, int release, int *erc) {
    MySqlObj *jdb = eval->jdb;
    Jsi_Interp *interp = jdb->interp;
    MysqlPrep *prep = eval->prep;
    SQLSIGASSERT(prep, STMT);
    MYSQL_STMT *myStmt = prep->myStmt;

    if (jdb->debug & mdbTMODE_STEP)
        JSI_DBQUERY_PRINTF( "DEBUG: step: %s\n", prep->zSql);
    int m = mysql_stmt_fetch(myStmt);
    if (m == MYSQL_NO_DATA)
        return JSI_BREAK;
    if (m) 
        return Jsi_LogError("fetch failed: %s", mysql_error(jdb->db));
    return JSI_OK;
#if 0
    if( eval->pArray ) {
        mdbEvalRowInfo(eval, 0, 0, 0);
    }
    if (release==0)
        return JSI_BREAK;
    eval->prep = 0;
    return JSI_OK;
#endif
}
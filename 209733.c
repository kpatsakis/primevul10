static void mdbReleaseStmt( MySqlObj *jdb, MysqlPrep *prep, int discard ) {
    if( jdb->maxStmts<=0 || discard ) {
        /* If the cache is turned off, deallocated the statement */
        mdbRelease1Stmt(jdb, prep);
    } else {
        /* Add the prepared statement to the beginning of the cache list. */
        if (!prep->elPtr)
            prep->elPtr = Jsi_ListPushFrontNew(jdb->stmtCache, prep);
        else
            Jsi_ListPushFront(jdb->stmtCache, prep->elPtr);
        mdbStmtLimit(jdb);
    }
}
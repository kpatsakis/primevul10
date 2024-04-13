static void dbReleaseStmt(
    Jsi_Db *jdb,                  /* Database handle */
    SqlPreparedStmt *pPreStmt,      /* Prepared statement handle to release */
    int discard                     /* True to delete (not cache) the pPreStmt */
) {
    //int i;
    //Jsi_Interp *interp = jdb->interp;

    /* Free the bound string and blob parameters */
    /*for(i=0; i<pPreStmt->nParm; i++) {
        Jsi_DecrRefCount(interp, pPreStmt->apParm[i]);
    }*/
    //pPreStmt->nParm = 0;

    if( jdb->stmtCacheMax<=0 || discard ) {
        /* If the cache is turned off, deallocated the statement */
        dbPrepStmtFree(jdb, pPreStmt);
    } else {
        /* Add the prepared statement to the beginning of the cache list, then limit. */
        if (!pPreStmt->elPtr)
            pPreStmt->elPtr = Jsi_ListPushFrontNew(jdb->stmtCache, pPreStmt);
        else
            Jsi_ListPushFront(jdb->stmtCache, pPreStmt->elPtr);
        dbPrepStmtLimit(jdb);
        jdb->stmtCacheCnt = Jsi_ListSize(jdb->stmtCache);
    }
}
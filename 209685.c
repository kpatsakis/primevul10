static Jsi_RC dbPrepareStmt(
    Jsi_Db *jdb,                  /* Database object */
    char const *zIn,                /* SQL to compile */
    char const **pzOut,             /* OUT: Pointer to next SQL statement */
    SqlPreparedStmt **ppPreStmt     /* OUT: Object used to cache statement */
) {
    const char *zSql = zIn;         /* Pointer to first SQL statement in zIn */
    sqlite3_stmt *pStmt;            /* Prepared statement object */
    SqlPreparedStmt *pPreStmt = 0;  /* Pointer to cached statement */
   // int nSql;                       /* Length of zSql in bytes */
    //int nVar;                       /* Number of variables in statement */
    //int iParm = 0;                  /* Next free entry in apParm */
    Jsi_RC rc = JSI_OK;
    Jsi_Interp *interp = jdb->interp;
    JSI_NOTUSED(interp);

    *ppPreStmt = 0;

    /* Trim spaces from the start of zSql and calculate the remaining length. */
    while( isspace(zSql[0]) ) {
        zSql++;
    }
    //nSql = Jsi_Strlen(zSql);
    Jsi_HashEntry *entry = Jsi_HashEntryFind(jdb->stmtHash, zSql);
    if (entry && ((pPreStmt = (SqlPreparedStmt*)Jsi_HashValueGet(entry)))) {
        
        if (jdb->debug & TMODE_PREPARE)
            JSI_DBQUERY_PRINTF( "DEBUG: prepare cache-hit: %s\n", zSql);
        pStmt = pPreStmt->pStmt;
        *pzOut = &zSql[pPreStmt->nSql];

        /* When a prepared statement is found, unlink it from the
        ** cache list.  It will later be added back to the beginning
        ** of the cache list in order to implement LRU replacement.
        */
        Jsi_ListPop(jdb->stmtCache, pPreStmt->elPtr);
        jdb->stmtCacheCnt = Jsi_ListSize(jdb->stmtCache);

    }

    /* If no prepared statement was found. Compile the SQL text. Also allocate
    ** a new SqlPreparedStmt structure.  */
    if( pPreStmt==0 ) {
        int nByte;

        if( SQLITE_OK!=sqlite3_prepare_v2(jdb->db, zSql, -1, &pStmt, pzOut) )
        
            return Jsi_LogError("PREPARE: %s", sqlite3_errmsg(jdb->db));
        if( pStmt==0 ) {
            if( SQLITE_OK!=sqlite3_errcode(jdb->db) ) {
                /* A compile-time error in the statement. */
                Jsi_LogError("PREP: %s", sqlite3_errmsg(jdb->db));
                return JSI_ERROR;
            } else {
                /* The statement was a no-op.  Continue to the next statement
                ** in the SQL string.
                */
                return JSI_OK;
            }
        }

        if (jdb->debug & TMODE_PREPARE)
            JSI_DBQUERY_PRINTF( "DEBUG: prepare new: %s\n", zSql);
        assert( pPreStmt==0 );
        //nVar = sqlite3_bind_parameter_count(pStmt);
        jdb->stmtCacheCnt++;
        nByte = sizeof(SqlPreparedStmt); // + nVar*sizeof(Jsi_Obj *);
        pPreStmt = (SqlPreparedStmt*)Jsi_Calloc(1, nByte);
        pPreStmt->sig = SQLITE_SIG_STMT;

        pPreStmt->pStmt = pStmt;
        pPreStmt->nSql = (*pzOut - zSql);
        pPreStmt->zSql = sqlite3_sql(pStmt);
        bool isNew = 0;
        pPreStmt->entry = Jsi_HashEntryNew(jdb->stmtHash, zSql, &isNew);
        if (!isNew)
            JSI_DBQUERY_PRINTF( "sqlite dup stmt entry");
        Jsi_HashValueSet(pPreStmt->entry, pPreStmt);
            
        //pPreStmt->apParm = (Jsi_Value **)&pPreStmt[1];
    }
    assert( pPreStmt );
    assert( Jsi_Strlen(pPreStmt->zSql)==pPreStmt->nSql );
    assert( 0==memcmp(pPreStmt->zSql, zSql, pPreStmt->nSql) );
    *ppPreStmt = pPreStmt;
    //pPreStmt->nParm = iParm; 
    return rc;
}
static Jsi_RC mdbPrepareStmt(MyDbEvalContext *p)
{
    MySqlObj *jdb = p->jdb;
    //int namedParams = !jdb->optPtr->noNamedParams;
    const char *zSql = p->zSql;         /* Pointer to first SQL statement in zIn */
    MYSQL_STMT *myStmt;            /* Prepared statement object */
    MysqlPrep *prep = 0;  /* Pointer to cached statement */
    Jsi_RC rc = JSI_OK;
    Jsi_Interp *interp = jdb->interp;


    Jsi_HashEntry *entry = Jsi_HashEntryFind(jdb->stmtHash, zSql);
    if (entry && ((prep = (MysqlPrep*)Jsi_HashValueGet(entry)))) {
        
        if (jdb->debug & mdbTMODE_PREPARE)
            JSI_DBQUERY_PRINTF( "DEBUG: prepare cache-hit: %s\n", zSql);
        myStmt = prep->myStmt;

        /* When a prepared statement is found, unlink it from the
        ** cache list.  It will later be added back to the beginning
        ** of the cache list in order to implement LRU replacement.
        */
        Jsi_ListPop(jdb->stmtCache, prep->elPtr);
        jdb->numStmts = Jsi_ListSize(jdb->stmtCache);
        
        // Sanity check for schema check: right now we just use number of columns
        if (prep->numCol != (int)mysql_stmt_field_count(prep->myStmt)) {
            mdbRelease1Stmt(jdb, prep);
            prep = NULL;
        }
    }
    
    /* If no prepared statement was found. Compile the SQL text. Also allocate
    ** a new MysqlPrep structure.  */
    if (!prep) {
        myStmt = mysql_stmt_init(jdb->db);
        if (!myStmt) 
            return Jsi_LogError("can't get statement: %s", mysql_error(jdb->db));
        char **paramNames;
        int paramCnt;
        int namedParams = 0;
        Jsi_DString *naStr = NULL;
        if (p->namedParams)
        {
            Jsi_RC rc = JSI_OK;
            Jsi_DString nsStr, nnStr;
            Jsi_DSInit(&nsStr);
            Jsi_DSInit(&nnStr);
            if (MySqlExtractParmNames(jdb, zSql, &nsStr, &nnStr) != JSI_OK)
                rc = Jsi_LogError("parsing names from query: %s", zSql);
            else if (Jsi_DSLength(&nnStr)) {
                namedParams = 1;
                zSql = Jsi_DSFreeDup(&nsStr);
                naStr = (Jsi_DString*)Jsi_Calloc(1, sizeof(*naStr));
                Jsi_SplitStr(Jsi_DSValue(&nnStr), &paramCnt, &paramNames, " ", naStr);
            }
            Jsi_DSFree(&nsStr);
            Jsi_DSFree(&nnStr);
            if (rc != JSI_OK)
                return rc;
        }

        if (mysql_stmt_prepare(myStmt, zSql, Jsi_Strlen(zSql)) )
        {
            Jsi_LogError("error in sql: %s", mysql_error(jdb->db));
            mysql_stmt_close(myStmt);
            if (namedParams) {
                Jsi_DSFree(naStr);
                Jsi_Free(naStr);
                Jsi_Free((char*)zSql);
            }
            return JSI_ERROR;
        }

        if (jdb->debug & mdbTMODE_PREPARE)
            JSI_DBQUERY_PRINTF( "DEBUG: prepare new: %s\n", zSql);
        assert( prep==0 );
        prep = (MysqlPrep*)Jsi_Calloc(1, sizeof(MysqlPrep));
        jdb->numStmts++;
        prep->sig = MYSQL_SIG_STMT;
        prep->myStmt = myStmt;
        if (!namedParams)
            prep->zSql = Jsi_Strdup(zSql);
        else {
            prep->zSql = (char*)zSql;
            prep->origSql = Jsi_Strdup(p->zSql);
            prep->naStr = naStr;
            prep->paramCnt = paramCnt;
            prep->paramNames = paramNames;
        }
        prep->paramMetaData = mysql_stmt_param_metadata(myStmt);
        prep->resultMetaData = mysql_stmt_result_metadata(myStmt);
        prep->numCol = mysql_stmt_field_count(myStmt);
        if (prep->numCol>0)
            prep->bindResult = (MYSQL_BIND *)Jsi_Calloc(prep->numCol, sizeof(MYSQL_BIND));
        prep->numParam = mysql_stmt_param_count(myStmt);
        if (prep->numParam>0 && !prep->bindParam) {
            prep->bindParam = (MYSQL_BIND *)Jsi_Calloc(prep->numParam, sizeof(MYSQL_BIND));
            prep->fieldParam = (SqlFieldResults*)Jsi_Calloc(prep->numParam, sizeof(*prep->fieldParam));
        }
        bool isNew = 0;
        prep->entry = Jsi_HashEntryNew(jdb->stmtHash, p->zSql, &isNew);
        if (!isNew)
            JSI_DBQUERY_PRINTF( "mysql dup stmt entry");
        Jsi_HashValueSet(prep->entry, prep);
    }
    p->prep = prep;
    return rc;
}
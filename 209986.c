static void mdbRelease1Stmt( MySqlObj *jdb, MysqlPrep *prep ) {
    // TODO: split out parts reusable by cached query.
    int i;
    if (prep->deleting)
        return;
    prep->deleting = 1;
    if (prep->myStmt)
        mysql_stmt_close(prep->myStmt);
    if (prep->resultMetaData)
        mysql_free_result(prep->resultMetaData);
    if (prep->paramMetaData)
        mysql_free_result(prep->paramMetaData);
    if (prep->bindParam)
        Jsi_Free(prep->bindParam);
    if (prep->fieldParam)
        Jsi_Free(prep->fieldParam);
    if (prep->bindResult) {
        for (i=0; i<prep->numCol; i++) {
            MYSQL_BIND *bind = prep->bindResult+i;
            if (bind->buffer_type == MYSQL_TYPE_STRING && bind->buffer)
                Jsi_Free(bind->buffer);
        }
        Jsi_Free(prep->bindResult);
    }
    if (prep->fieldResult)
        Jsi_Free(prep->fieldResult);
    if (prep->colTypes)
        Jsi_Free(prep->colTypes);
    if (prep->colNames)
        Jsi_Free(prep->colNames);
    if (prep->zSql)
        Jsi_Free(prep->zSql);
    if (prep->naStr) {
        Jsi_Free(prep->origSql);
        Jsi_DSFree(prep->naStr);
        Jsi_Free(prep->naStr);
    }
    if (prep->entry)
        Jsi_HashEntryDelete(prep->entry);
    if (prep->elPtr) {
        Jsi_ListEntry *pPtr = prep->elPtr;
        prep->elPtr = NULL;
        Jsi_ListEntryDelete(pPtr);
    }
    Jsi_Free(prep);
    jdb->numStmts--;
}
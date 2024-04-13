static Jsi_RC dbStmtFreeProc(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *value) {
    Jsi_Db *jdb = (Jsi_Db*)interp;
    Jsi_ListEntry *l = (Jsi_ListEntry*)hPtr;
    SqlPreparedStmt *prep = (SqlPreparedStmt *)Jsi_ListValueGet(l);
    prep->elPtr = NULL;
    dbPrepStmtFree(jdb, prep);
    return JSI_OK;
}
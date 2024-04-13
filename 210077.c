static Jsi_RC mdbStmtFreeProc(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *value) {
    MySqlObj *jdb = (MySqlObj*)interp;
    Jsi_ListEntry *l = (Jsi_ListEntry*)hPtr;
    mdbRelease1Stmt(jdb, (MysqlPrep*)Jsi_ListValueGet(l));
    return JSI_OK;
}
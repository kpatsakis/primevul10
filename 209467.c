static Jsi_RC SqliteOnecolumnCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                          Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_RC rc;
    Jsi_Db *jdb;
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *vSql = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_DString sStr;
    Jsi_DSInit(&sStr);
    DbEvalContext sEval = {};
    const char *zSql = Jsi_ValueGetDString(interp, vSql, &dStr, 0);

    sEval.nocache = jdb->queryOpts.nocache;
    if ((rc = dbEvalInit(interp, &sEval, jdb, zSql, &sStr, 0, 0)) != JSI_OK)
        return rc;
    sEval.ret = *ret;
    sEval.tocall = NULL;
    int cnt = 0;


    if( JSI_OK==(rc = dbEvalStep(&sEval)) ) {
        sqlite3_stmt *pStmt = sEval.pPreStmt->pStmt;
        int nCol = sqlite3_column_count(pStmt);
        if (nCol>0)
            dbEvalSetColumnValue(&sEval, 0, ret);
        cnt++;
    }
    dbEvalFinalize(&sEval);
    if( rc==JSI_BREAK ) {
        rc = JSI_OK;
    }
    Jsi_DSFree(&dStr);
    return rc;
}
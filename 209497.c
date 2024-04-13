static Jsi_RC dbEvalStepOption(DbEvalContext *p, OptionBind *obPtr, int *cntPtr, int dataIdx, int bindMax, Jsi_CDataDb *dbopts, int *erc) {
    Jsi_Db *jdb = p->jdb;
    int cnt = 0;
    while( p->zSql[0] || p->pPreStmt ) {
        Jsi_RC rc;
        cnt++;
        if( p->pPreStmt==0 ) {
            rc = dbPrepareStmt(p->jdb, p->zSql, &p->zSql, &p->pPreStmt);
            if( rc!=JSI_OK ) return rc;
        }
        if (bindMax!=0) {
            rc = dbBindOptionStmt(jdb, p->pPreStmt->pStmt, obPtr, dataIdx, bindMax, dbopts);
            if( rc!=JSI_OK ) return rc;
        }
        rc = dbEvalStepSub(p, 1, erc);
        if (rc != JSI_BREAK)
            return rc;
        *cntPtr = cnt;
    }
    
    /* Finished */
    return JSI_BREAK;
}
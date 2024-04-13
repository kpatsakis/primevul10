static Jsi_RC dbEvalStep(DbEvalContext *p) {
    while( p->zSql[0] || p->pPreStmt ) {
        Jsi_RC rc;
        if( p->pPreStmt==0 ) {
            rc = dbPrepareAndBind(p->jdb, p->zSql, &p->zSql, &p->pPreStmt);
            if( rc!=JSI_OK ) return rc;
        }
        rc = dbEvalStepSub(p, 1, NULL);
        if (rc != JSI_BREAK)
            return rc;
    }
    
    /* Finished */
    return JSI_BREAK;
}
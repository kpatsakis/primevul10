static int dbSqlCollate(
    void *pCtx,
    int nA,
    const void *zA,
    int nB,
    const void *zB
) {
    SqlCollate *p = (SqlCollate *)pCtx;
    Jsi_Interp *interp = p->interp;

    int rc;
    //Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Value *vpargs, *items[3], *ret;

    items[0] = Jsi_ValueNewObj(interp, p->jdb->fobj);
    items[1] = Jsi_ValueMakeStringDup(interp, NULL, (char*)zA);
    items[2] = Jsi_ValueMakeStringDup(interp, NULL, (char*)zB);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 3, 0));
    ret = Jsi_ValueNew1(interp);
    rc = Jsi_FunctionInvoke(interp, p->zScript, vpargs, &ret, NULL);
    if( JSI_OK!=rc ) {
        //jdb->errCnt++;
        rc = 0;
    } else
        rc = dbGetIntBool(interp, ret);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
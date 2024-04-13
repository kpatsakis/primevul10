static int dbWalHandler( void *cd, sqlite3 *db, const char *zDb, int nEntry ){
    int rc;
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Interp *interp = jdb->interp;
    Jsi_Value *vpargs, *items[3] = {}, *ret;

    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[1] = Jsi_ValueMakeStringDup(interp, NULL, zDb);
    items[2] = Jsi_ValueMakeNumber(interp, NULL, (Jsi_Number)nEntry);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 3, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onWalHook, vpargs, &ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    if (rc != JSI_OK) {
        jdb->errCnt++;
        rc = 1;
    } else
        rc = dbGetIntBool(jdb->interp, ret);
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
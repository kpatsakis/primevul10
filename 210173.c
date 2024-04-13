static int dbProgressHandler(void *cd) {
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Value *vpargs, *items[3] = {}, *ret;
    Jsi_Interp *interp = jdb->interp;

    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 1, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew1(interp);
    int rc = Jsi_FunctionInvoke(interp, jdb->onProgress, vpargs, &ret, NULL);
    if( JSI_OK!=rc ) {
        jdb->errCnt++;
        rc = 1;
    } else
        rc = dbGetIntBool(interp, ret);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
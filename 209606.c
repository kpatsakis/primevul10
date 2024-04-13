static int dbBusyHandler(void *cd, int nTries) {
    int rc;
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Value *vpargs, *items[3] = {}, *ret;
    Jsi_Interp *interp = jdb->interp;

    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[1] = Jsi_ValueMakeNumber(interp, NULL, (Jsi_Number)nTries);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 2, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew1(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onBusy, vpargs, &ret, NULL);
    if( JSI_OK!=rc ) {
        jdb->errCnt++;
        rc = 1;
    } else
        rc = dbGetIntBool(interp, ret);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
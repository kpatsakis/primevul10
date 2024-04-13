static int dbCommitHandler(void *cd) {
    int rc = 0;
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Interp *interp = jdb->interp;
    Jsi_Value *vpargs, *items[2] = {}, *ret = Jsi_ValueNew1(interp);
    
    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 1, 0));
    Jsi_IncrRefCount(interp, vpargs);
    if( JSI_OK!=Jsi_FunctionInvoke(interp, jdb->onCommit, NULL, &ret, NULL) ) {
        jdb->errCnt++;
        rc = 1;
    } else
        rc = dbGetIntBool(interp, ret);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
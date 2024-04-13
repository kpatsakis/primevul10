static int dbAuthCallback(
    void *pArg,
    int code,
    const char *zArg1,
    const char *zArg2,
    const char *zArg3,
    const char *zArg4
) {
    const char *zCode;
    int rc;
    const char *zReply;
    Jsi_Db *jdb = (Jsi_Db*)pArg;
    Jsi_Interp *interp = jdb->interp;
    if( jdb->disableAuth ) return SQLITE_OK;

    switch( code ) {
    case SQLITE_COPY              :
        zCode="SQLITE_COPY";
        break;
    case SQLITE_CREATE_INDEX      :
        zCode="SQLITE_CREATE_INDEX";
        break;
    case SQLITE_CREATE_TABLE      :
        zCode="SQLITE_CREATE_TABLE";
        break;
    case SQLITE_CREATE_TEMP_INDEX :
        zCode="SQLITE_CREATE_TEMP_INDEX";
        break;
    case SQLITE_CREATE_TEMP_TABLE :
        zCode="SQLITE_CREATE_TEMP_TABLE";
        break;
    case SQLITE_CREATE_TEMP_TRIGGER:
        zCode="SQLITE_CREATE_TEMP_TRIGGER";
        break;
    case SQLITE_CREATE_TEMP_VIEW  :
        zCode="SQLITE_CREATE_TEMP_VIEW";
        break;
    case SQLITE_CREATE_TRIGGER    :
        zCode="SQLITE_CREATE_TRIGGER";
        break;
    case SQLITE_CREATE_VIEW       :
        zCode="SQLITE_CREATE_VIEW";
        break;
    case SQLITE_DELETE            :
        zCode="SQLITE_DELETE";
        break;
    case SQLITE_DROP_INDEX        :
        zCode="SQLITE_DROP_INDEX";
        break;
    case SQLITE_DROP_TABLE        :
        zCode="SQLITE_DROP_TABLE";
        break;
    case SQLITE_DROP_TEMP_INDEX   :
        zCode="SQLITE_DROP_TEMP_INDEX";
        break;
    case SQLITE_DROP_TEMP_TABLE   :
        zCode="SQLITE_DROP_TEMP_TABLE";
        break;
    case SQLITE_DROP_TEMP_TRIGGER :
        zCode="SQLITE_DROP_TEMP_TRIGGER";
        break;
    case SQLITE_DROP_TEMP_VIEW    :
        zCode="SQLITE_DROP_TEMP_VIEW";
        break;
    case SQLITE_DROP_TRIGGER      :
        zCode="SQLITE_DROP_TRIGGER";
        break;
    case SQLITE_DROP_VIEW         :
        zCode="SQLITE_DROP_VIEW";
        break;
    case SQLITE_INSERT            :
        zCode="SQLITE_INSERT";
        break;
    case SQLITE_PRAGMA            :
        zCode="SQLITE_PRAGMA";
        break;
    case SQLITE_READ              :
        zCode="SQLITE_READ";
        break;
    case SQLITE_SELECT            :
        zCode="SQLITE_SELECT";
        break;
    case SQLITE_TRANSACTION       :
        zCode="SQLITE_TRANSACTION";
        break;
    case SQLITE_UPDATE            :
        zCode="SQLITE_UPDATE";
        break;
    case SQLITE_ATTACH            :
        zCode="SQLITE_ATTACH";
        break;
    case SQLITE_DETACH            :
        zCode="SQLITE_DETACH";
        break;
    case SQLITE_ALTER_TABLE       :
        zCode="SQLITE_ALTER_TABLE";
        break;
    case SQLITE_REINDEX           :
        zCode="SQLITE_REINDEX";
        break;
    case SQLITE_ANALYZE           :
        zCode="SQLITE_ANALYZE";
        break;
    case SQLITE_CREATE_VTABLE     :
        zCode="SQLITE_CREATE_VTABLE";
        break;
    case SQLITE_DROP_VTABLE       :
        zCode="SQLITE_DROP_VTABLE";
        break;
    case SQLITE_FUNCTION          :
        zCode="SQLITE_FUNCTION";
        break;
    case SQLITE_SAVEPOINT         :
        zCode="SQLITE_SAVEPOINT";
        break;
    default                       :
        zCode="????";
        break;
    }
    int i = 0;
    Jsi_Value *vpargs, *items[10] = {}, *ret;
    items[i++] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zCode);
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zArg1 ? zArg1 : "");
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zArg2 ? zArg2 : "");
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zArg3 ? zArg3 : "");
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zArg4 ? zArg4 : "");
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, i, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onAuth, vpargs, &ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);

    if (rc == JSI_OK && (zReply = Jsi_ValueGetStringLen(interp, ret, NULL)))
    {
        if( Jsi_Strcmp(zReply,"SQLITE_OK")==0 ) {
            rc = SQLITE_OK;
        } else if( Jsi_Strcmp(zReply,"SQLITE_DENY")==0 ) {
            rc = SQLITE_DENY;
        } else if( Jsi_Strcmp(zReply,"SQLITE_IGNORE")==0 ) {
            rc = SQLITE_IGNORE;
        } else {
            rc = 999;
        }
    }
    Jsi_DecrRefCount(interp, ret);
    return rc;
}
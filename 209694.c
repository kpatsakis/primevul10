static Jsi_RC MySqlConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_LogTest("Creating new MySql");
        
    jdb = (MySqlObj*)Jsi_Calloc(1, sizeof(*jdb));
    SQLSIGINIT(jdb, DB);
    const char *groupname = "mysqljsi";
    jdb->_ = &mydbObjCmd;
    jdb->_->newCnt++;
    jdb->_->activeCnt++;

    jdb->maxStmts = NUM_PREPARED_STMTS;
    jdb->forceInt = 1;
    jdb->interp = interp;
    jdb->hasOpts = (arg != NULL && !Jsi_ValueIsNull(interp,arg));
    if (jdb->hasOpts && Jsi_OptionsProcess(interp, SqlOptions, jdb, arg, 0) < 0) {
        jdb->deleted = 1;
        mysqlObjFree(interp, jdb);
        return JSI_ERROR;
    }
    if (!jdb->udata) {
        jdb->udata = Jsi_ValueNewObj(interp, NULL);
        Jsi_IncrRefCount(interp, jdb->udata);
    }
    jdb->db = mysql_init(NULL);
    jdb->version = (MYSQL_VERSION_MAJOR + ((Jsi_Number)MYSQL_VERSION_MINOR/100.0) + ((Jsi_Number)MYSQL_VERSION_PATCH/10000.0));

#if (MYSQL_VERSION_ID>=32350)
    if (jdb->reconnect)
    {
      my_bool reconnect = 1;
      mysql_options(jdb->db, MYSQL_OPT_RECONNECT, &reconnect);
    }
    mysql_options(jdb->db, MYSQL_READ_DEFAULT_GROUP, groupname);
#endif

#if (MYSQL_VERSION_ID >= 40107)
    if (jdb->sslKey) {
        const char *sslcert = Jsi_ValueString(interp, jdb->sslCert, NULL),
            *sslca = Jsi_ValueString(interp, jdb->sslCA, NULL),
            *sslcapath = Jsi_ValueString(interp, jdb->sslCAPath, NULL),
            *sslcipher = Jsi_ValueString(interp, jdb->sslCipher, NULL),
            *sslkey = Jsi_ValueString(interp, jdb->sslKey, NULL);
        mysql_ssl_set(jdb->db, sslkey, sslcert, sslca, sslcapath, sslcipher);
        jdb->dbflags |= CLIENT_SSL;
    }
#endif

    if (!mdbConnect(interp, jdb)) {
        Jsi_LogError("connect failed %s", mysql_error(jdb->db));
        mysqlObjFree(interp, jdb);
        return JSI_ERROR;
    }

    if (jdb->enableMulti) {
        if (mysql_set_server_option(jdb->db, MYSQL_OPTION_MULTI_STATEMENTS_ON))
            Jsi_LogWarn("multi on failed %s", mysql_error(jdb->db));
    }
    //jdb->event = Jsi_EventNew(interp, mysqlUpdate, jdb); //TODO: events
    Jsi_Value *toacc = NULL;
    if (Jsi_FunctionIsConstructor(funcPtr)) {
        toacc = _this;
    } else {
        Jsi_Obj *o = Jsi_ObjNew(interp);
        Jsi_PrototypeObjSet(interp, "MySql", o);
        Jsi_ValueMakeObject(interp, ret, o);
        toacc = *ret;
    }

    Jsi_Obj *fobj = Jsi_ValueGetObj(interp, toacc);
    if ((jdb->objId = Jsi_UserObjNew(interp, &mysqlobject, fobj, jdb))<0) {
        mysqlObjFree(interp, jdb);
        Jsi_ValueMakeUndef(interp, ret);
        return JSI_ERROR;
    }
    jdb->stmtHash = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    jdb->userObjPtr = fobj;
    jdb->optPtr = &jdb->queryOpts;
    jdb->stmtCache = Jsi_ListNew((Jsi_Interp*)jdb, 0, mdbStmtFreeProc);
    return JSI_OK;
}
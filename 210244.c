static Jsi_RC SqliteConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                             Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_RC rc = JSI_ERROR;
    /* void *cd = clientData; */
    int  flags;
    char *zErrMsg;
    const char *zFile = NULL, *vfs = 0;
    /* In normal use, each JSI interpreter runs in a single thread.  So
    ** by default, we can turn of mutexing on SQLite database connections.
    ** However, for testing purposes it is useful to have mutexes turned
    ** on.  So, by default, mutexes default off.  But if compiled with
    ** SQLITE_JSI_DEFAULT_FULLMUTEX then mutexes default on.
    */
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_JSI_DEFAULT_FULLMUTEX
    flags |= SQLITE_OPEN_FULLMUTEX;
#else
    flags |= SQLITE_OPEN_NOMUTEX;
#endif

    Jsi_Value *vFile = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 1);
    Jsi_DString dStr = {};
    int ismem = 0, create = 0;
    Jsi_Obj *fobj;
    Jsi_Value *toacc;
    const char *vf;
    const char *dbname = NULL;
    
    if (vFile==NULL || Jsi_ValueIsNull(interp, vFile) ||
        ((vf = Jsi_ValueString(interp, vFile, NULL)) && !Jsi_Strcmp(vf,":memory:"))) {
        zFile = ":memory:";
        ismem = 1;
    } else {
        zFile = Jsi_ValueNormalPath(interp, vFile, &dStr);
        if (zFile == NULL) 
            return Jsi_LogError("bad or missing file name");
        Jsi_StatBuf st = {};
        st.st_uid = -1;
        create = (Jsi_Lstat(interp, vFile, &st) != 0);
    }
    zErrMsg = 0;
    Jsi_Db *db = (Jsi_Db*)Jsi_Calloc(1, sizeof(*db) );
    if( db==0 ) {
        Jsi_DSFree(&dStr);
        Jsi_LogError("malloc failed");
        return JSI_ERROR;
    }
    db->sig = SQLITE_SIG_DB;
    db->_ = &dbObjCmd;
    db->_->newCnt++;
    db->_->activeCnt++;
    db->stmtCacheMax = NUM_PREPARED_STMTS;
    db->hasOpts = 1;
    if ((arg != NULL && !Jsi_ValueIsNull(interp,arg))
        && Jsi_OptionsProcess(interp, SqlOptions, db, arg, 0) < 0) {
        rc = JSI_ERROR;
        goto bail;
    }
    if (ismem == 0 &&
        (Jsi_InterpAccess(interp, vFile, (db->readonly ? JSI_INTACCESS_READ : JSI_INTACCESS_WRITE)) != JSI_OK
        || (create && Jsi_InterpAccess(interp, vFile, JSI_INTACCESS_CREATE) != JSI_OK))) {
        Jsi_LogError("Safe accces denied");
        goto bail;
    }

    if (db->stmtCacheMax<0 || db->stmtCacheMax>MAX_PREPARED_STMTS) {
        Jsi_LogError("option stmtCacheMax value %d is not in range 0..%d", db->stmtCacheMax, MAX_PREPARED_STMTS);
        goto bail;
    }
    if (!db->udata) {
        db->udata = Jsi_ValueNewObj(interp, NULL);
        Jsi_IncrRefCount(interp, db->udata);
    }
    if (db->readonly) {
        flags &= ~(SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
        flags |= SQLITE_OPEN_READONLY;
    } else {
        flags &= ~SQLITE_OPEN_READONLY;
        flags |= SQLITE_OPEN_READWRITE;
        if (db->noCreate) {
            flags &= ~SQLITE_OPEN_CREATE;
        }
    }
    if (db->vfs)
        vfs = Jsi_ValueToString(interp, db->vfs, NULL);
    if(db->mutex == MUTEX_NONE) {
        flags |= SQLITE_OPEN_NOMUTEX;
        flags &= ~SQLITE_OPEN_FULLMUTEX;
    } else {
        flags &= ~SQLITE_OPEN_NOMUTEX;
    }
    if(db->mutex ==MUTEX_FULL) {
        flags |= SQLITE_OPEN_FULLMUTEX;
        flags &= ~SQLITE_OPEN_NOMUTEX;
    } else {
        flags &= ~SQLITE_OPEN_FULLMUTEX;
    }
  
    if (SQLITE_OK != sqlite3_open_v2(zFile, &db->db, flags, vfs)) {
        Jsi_LogError("db open failed: %s", zFile);
        goto bail;
    }
    //Jsi_DSFree(&translatedFilename);

    if( SQLITE_OK!=sqlite3_errcode(db->db) ) {
        zErrMsg = sqlite3_mprintf("%s", sqlite3_errmsg(db->db));
        DbClose(db->db);
        db->db = 0;
    }
    if( db->db==0 ) {
        sqlite3_free(zErrMsg);
        goto bail;
    }
;
    toacc = NULL;
    if (Jsi_FunctionIsConstructor(funcPtr)) {
        toacc = _this;
    } else {
        Jsi_Obj *o = Jsi_ObjNew(interp);
        Jsi_PrototypeObjSet(interp, "Sqlite", o);
        Jsi_ValueMakeObject(interp, ret, o);
        toacc = *ret;
    }
    sqlite3_create_function(db->db, "unixtime", -1, SQLITE_UTF8, db, jsiSqlFuncUnixTime, 0, 0);
    
    fobj = Jsi_ValueGetObj(interp, toacc /* constructor obj*/);
    if ((db->objId = Jsi_UserObjNew(interp, &sqliteobject, fobj, db))<0)
        goto bail;
    db->stmtHash = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    db->fobj = fobj;
    //dbSys->cnt = Jsi_UserObjCreate(interp, sqliteobject.name /*dbSys*/, fobj, db);
    db->interp = interp;
    db->optPtr = &db->queryOpts;
    db->stmtCache = Jsi_ListNew((Jsi_Interp*)db, 0, dbStmtFreeProc);
    rc = JSI_OK;
    dbname = Jsi_DSValue(&db->name);
    if (dbname[0])
        sqlite3_db_config(db->db, SQLITE_DBCONFIG_MAINDBNAME, dbname);
    Jsi_JSONParseFmt(interp, &db->version, "{libVer:\"%s\", hdrVer:\"%s\", hdrNum:%d, hdrSrcId:\"%s\", pkgVer:%d}",
        (char *)sqlite3_libversion(), SQLITE_VERSION, SQLITE_VERSION_NUMBER, SQLITE_SOURCE_ID, jsi_DbPkgVersion); 
    dbSetupCallbacks(db, NULL);
    
bail:
    if (rc != JSI_OK) {
        if (db->hasOpts)
            Jsi_OptionsFree(interp, SqlOptions, db, 0);
        db->_->activeCnt--;
        Jsi_Free(db);
    }
    Jsi_DSFree(&dStr);
    Jsi_ValueMakeUndef(interp, ret);
    return rc;
}
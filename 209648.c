Jsi_Db* Jsi_DbNew(const char *zFile, int inFlags /* JSI_DBI_* */)
{
    char *zErrMsg;
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#ifdef SQLITE_JSI_DEFAULT_FULLMUTEX
    flags |= SQLITE_OPEN_FULLMUTEX;
#else
    flags |= SQLITE_OPEN_NOMUTEX;
#endif
    if (!zFile)
        zFile = ":memory:";
    zErrMsg = 0;
    Jsi_Db *db = (Jsi_Db*)Jsi_Calloc(1, sizeof(*db) );
    if( db==0 ) {
        JSI_DBQUERY_PRINTF( "malloc failed\n");
        return NULL;
    }
    db->sig = SQLITE_SIG_DB;
    db->stmtCacheMax = NUM_PREPARED_STMTS;
    db->optPtr = &db->queryOpts;

    if (inFlags&JSI_DBI_READONLY) {
        flags &= ~(SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
        flags |= SQLITE_OPEN_READONLY;
    } else {
        flags &= ~SQLITE_OPEN_READONLY;
        flags |= SQLITE_OPEN_READWRITE;
        if (inFlags&JSI_DBI_NOCREATE) {
            flags &= ~SQLITE_OPEN_CREATE;
        }
    }
    if(inFlags&JSI_DBI_NO_MUTEX) {
        flags |= SQLITE_OPEN_NOMUTEX;
        flags &= ~SQLITE_OPEN_FULLMUTEX;
    } else {
        flags &= ~SQLITE_OPEN_NOMUTEX;
    }
    if(inFlags&JSI_DBI_FULL_MUTEX) {
        flags |= SQLITE_OPEN_FULLMUTEX;
        flags &= ~SQLITE_OPEN_NOMUTEX;
    } else {
        flags &= ~SQLITE_OPEN_FULLMUTEX;
    }
    char cpath[PATH_MAX];
    char *npath = Jsi_FileRealpathStr(NULL, zFile, cpath);
    
    if (SQLITE_OK != sqlite3_open_v2(npath, &db->db, flags, NULL)) {
        JSI_DBQUERY_PRINTF( "db open failed: %s\n", npath);
        goto bail;
    }
    //Jsi_DSFree(&translatedFilename);

    if( SQLITE_OK!=sqlite3_errcode(db->db) ) {
        zErrMsg = sqlite3_mprintf("%s", sqlite3_errmsg(db->db));
        DbClose(db->db);
        db->db = 0;
    }
    if( db->db==0 ) {
        JSI_DBQUERY_PRINTF( "Db open failed %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        goto bail;
    }
    db->stmtHash = Jsi_HashNew(NULL, JSI_KEYS_STRING, NULL);
    db->strKeyTbl = Jsi_HashNew(NULL, JSI_KEYS_STRING, NULL);
    db->stmtCache = Jsi_ListNew((Jsi_Interp*)db, 0, dbStmtFreeProc);
    return db;
    
bail:
    return NULL;
}
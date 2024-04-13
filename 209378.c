Jsi_RC Jsi_InitSqlite(Jsi_Interp *interp, int release)
{
    if (release) return Jsi_DoneSqlite(interp);
    Jsi_Hash* dbSys;
#if JSI_USE_STUBS
    if (Jsi_StubsInit(interp, 0) != JSI_OK)
        return JSI_ERROR;
#endif

    if (!(dbSys = Jsi_UserObjRegister(interp, &sqliteobject))) 
        return Jsi_LogError("Failed to init sqlite extension");

    Jsi_Value *info = Jsi_ValueNew1(interp);
    Jsi_JSONParseFmt(interp, &info, "{libVer:\"%s\", hdrVer:\"%s\", hdrNum:%d, hdrSrcId:\"%s\", pkgVer:%d}",
        (char *)sqlite3_libversion(), SQLITE_VERSION, SQLITE_VERSION_NUMBER, SQLITE_SOURCE_ID, jsi_DbPkgVersion);
    Jsi_PkgOpts dbPkgOpts = { db_ObjCmd_Specs, &dbObjCmd, sqliteCmds, info };
    Jsi_RC rc = Jsi_PkgProvideEx(interp, "Sqlite", jsi_DbPkgVersion, Jsi_InitSqlite, &dbPkgOpts);
    Jsi_DecrRefCount(interp, info);
    if (rc != JSI_OK)
        return JSI_ERROR;

    if (!Jsi_CommandCreateSpecs(interp, sqliteobject.name, sqliteCmds, dbSys, JSI_CMDSPEC_ISOBJ))
        return JSI_ERROR;        

#ifdef JSI_DB_TEST
    if (getenv("RUN_DB_TEST"))
        TestSqlite(interp);
#endif
    jsi_DbVfs **dbVfsPtrPtr = (jsi_DbVfs **)Jsi_InterpGetData(interp, JSI_SQLITE_DB_VFS, NULL);
    if (dbVfsPtrPtr)
        *dbVfsPtrPtr = &SqliteDbVfs;
    return JSI_OK;
}
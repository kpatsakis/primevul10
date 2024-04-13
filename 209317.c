Jsi_RC Jsi_InitMySql(Jsi_Interp *interp, int release)
{
    if (release) {
        if (!--mydbObjCmd.init)
            mysql_library_end();
        return Jsi_DoneMySql(interp);
    }
    Jsi_Hash* dbSys;
#if JSI_USE_STUBS
  if (Jsi_StubsInit(interp, 0) != JSI_OK)
    return JSI_ERROR;
#endif
#ifndef JSI_OMIT_THREADS
    if (mydbObjCmd.init == 0 && mysql_library_init(0, NULL, NULL))
        return Jsi_LogError("failed to initialize MySQL library\n");
#else
    return Jsi_LogError("Threads required for mysql");
#endif

    Jsi_Value *info = Jsi_ValueNew1(interp);
    Jsi_JSONParseFmt(interp, &info, "{pkgVer:%d}", MYSQL_VERSION_ID);
    Jsi_PkgOpts dbPkgOpts = { mydb_ObjCmd_Specs, &mydbObjCmd, mysqlCmds, info};
    Jsi_RC rc = Jsi_PkgProvideEx(interp, "MySql", 1.1, Jsi_InitMySql, &dbPkgOpts);
    Jsi_DecrRefCount(interp, info);
    if (rc != JSI_OK)
        rc = JSI_ERROR;
    else if (!(dbSys = Jsi_UserObjRegister(interp, &mysqlobject))) 
        rc = Jsi_LogError("Failed to init mysql extension");
    else if (!Jsi_CommandCreateSpecs(interp, mysqlobject.name, mysqlCmds, dbSys, JSI_CMDSPEC_ISOBJ))
        rc = JSI_ERROR;
    if (rc == JSI_OK)
        mydbObjCmd.init++;
    else
        mysql_library_end();
    return rc;
}
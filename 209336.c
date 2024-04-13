static Jsi_RC Jsi_DoneSqlite(Jsi_Interp *interp)
{
    Jsi_UserObjUnregister(interp, &sqliteobject);
    const char *provide = "Sqlite";
    Jsi_PkgProvide(interp, provide, -1, NULL);
    return JSI_OK;
}
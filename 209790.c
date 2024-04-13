Jsi_RC Jsi_initSqlite(Jsi_Interp *interp, int release)
{
    if (!release) Jsi_InterpSetData(interp, JSI_SQLITE_DB_VFS, &jsi_dbVfsPtr, NULL);
    return JSI_OK;
}
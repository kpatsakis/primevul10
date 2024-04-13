Jsi_RC Jsi_doneSqlite(Jsi_Interp *interp)
{
    Jsi_InterpFreeData(interp, JSI_SQLITE_DB_VFS);
    return JSI_OK;
}
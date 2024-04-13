Jsi_Db* Jsi_DbNew(const char *zFile, int inFlags /* JSI_DBI_* */)
{
    if (!jsi_dbVfsPtr) {
        printf( "Sqlite unsupported\n");
        return NULL;
    }
    return jsi_dbVfsPtr->dbNew(zFile, inFlags);
}
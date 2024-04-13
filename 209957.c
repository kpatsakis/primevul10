void *Jsi_DbHandle(Jsi_Interp *interp, Jsi_Db* jdb)
{
    if (!jsi_dbVfsPtr) {
        printf( "Sqlite unsupported\n");
        return NULL;
    }
    return jsi_dbVfsPtr->dbHandle(interp, jdb);
}
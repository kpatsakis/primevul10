Jsi_DbQuery(Jsi_Db *jdb, Jsi_CDataDb *dPtr, const char *query)
{
    if (!jsi_dbVfsPtr) {
        printf( "Sqlite unsupported\n");
        return -1;
    }
    return jsi_dbVfsPtr->dbcQuery(jdb, dPtr, query);
}
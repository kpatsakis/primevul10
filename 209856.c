static int dbExecCmd(Jsi_Db *jdb, const char *zQuery, int *erc)
{
    int rc = sqlite3_exec(jdb->db, zQuery, 0, 0, 0);
    if (rc == SQLITE_BUSY) {
        if (erc) *erc = -2;
    } else if (rc != SQLITE_OK) {
        if (erc) *erc = -1;
    } else
        return 1;
    return 0;
}
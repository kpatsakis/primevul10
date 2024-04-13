static MYSQL* mdbConnect(Jsi_Interp *interp, MySqlObj* jdb)
{
    return mysql_real_connect(jdb->db,
        jdb->host?Jsi_ValueString(interp, jdb->host, NULL):NULL,
        jdb->user, jdb->password, jdb->database,
        jdb->port, 0, jdb->dbflags);
}
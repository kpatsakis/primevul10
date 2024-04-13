static void mysqlObjErase(MySqlObj *jdb)
{
    mdbDeleteCmd(jdb);
    jdb->db = NULL;
}
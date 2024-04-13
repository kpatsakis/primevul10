static bool mysqlObjIsTrue(void *data)
{
    MySqlObj *jdb = (MySqlObj*)data;
    SQLSIGASSERT(jdb,DB);
    if (!jdb->db) return 0;
    else return 1;
}
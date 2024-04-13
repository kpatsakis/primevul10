long long mdbLastInsertRowid(MySqlObj* jdb)
{
    return mysql_insert_id(jdb->db);
}
static void sqliteObjErase(Jsi_Db *jdb)
{
    dbDeleteCmd(jdb);
    jdb->db = NULL;
}
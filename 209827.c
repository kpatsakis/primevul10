static bool sqliteObjIsTrue(void *data)
{
    Jsi_Db *db = (Jsi_Db*)data;
    SQLSIGASSERT(db,DB);
    if (!db->db) return 0;
    else return 1;
}
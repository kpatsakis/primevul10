void *Jsi_DbHandle(Jsi_Interp *interp, Jsi_Db* jdb)
{
    SQLSIGASSERT(jdb,DB);
    return jdb->db;
}
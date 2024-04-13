Jsi_DbQuery(Jsi_Db *jdb, Jsi_CDataDb *dbopts, const char *query)
{
    int rc = jsi_DbQuery(jdb, dbopts, query);
#ifdef JSI_DBQUERY_ERRORCMD
    if (rc<0)
        rc = JSI_DBQUERY_ERRORCMD(jdb, specs, data, arrSize, query, dopts, rc);
#endif
    return rc;
}
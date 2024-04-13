static Jsi_RC dbEvalInit(
    Jsi_Interp *interp,
    DbEvalContext *p,               /* Pointer to structure to initialize */
    Jsi_Db *jdb,                  /* Database handle */
    const char* zSql,                /* Value containing SQL script */
    Jsi_DString *dStr,
    Jsi_Obj *pArray,                /* Name of Jsi array to set (*) element of */
    Jsi_Obj *pValVar                  /* Name element in array for list. */
) {
    p->dSql = dStr;
    p->zSql = Jsi_DSAppend(p->dSql, zSql?zSql:"", NULL);
    p->jdb = jdb;
    return JSI_OK;
}
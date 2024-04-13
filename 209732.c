static Jsi_RC mdbEvalInit(
    Jsi_Interp *interp,
    MyDbEvalContext *p,               /* Pointer to structure to initialize */
    MySqlObj *jdb,                  /* Database handle */
    const char* zSql,                /* Value containing SQL script */
    Jsi_DString *dStr,
    Jsi_Obj *pArray,                /* Name of Jsi array to set (*) element of */
    Jsi_Obj *pValVar                  /* Name element in array for list. */
) {
    p->dzSql = dStr;
    p->zSql = Jsi_DSAppend(p->dzSql, zSql?zSql:"", NULL);
    p->jdb = jdb;
    return JSI_OK;
}
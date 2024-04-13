static Jsi_RC MySqlOnecolumnCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                          Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_RC rc;
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Value *vSql = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_DString sStr;
    Jsi_DSInit(&sStr);
    MyDbEvalContext sEval = {};
    const char *zSql = Jsi_ValueGetDString(interp, vSql, &dStr, 0);

    sEval.nocache = jdb->queryOpts.nocache;
    if (mdbEvalInit(interp, &sEval, jdb, zSql, &sStr, 0, 0) != JSI_OK)
        return JSI_ERROR;
    sEval.ret = *ret;
    sEval.tocall = NULL;
    int cnt = 0;


    if( JSI_OK==(rc = mdbEvalStep(&sEval)) ) {
        int nCol = sEval.prep->numCol;
        if (nCol>0)
            mdbEvalSetColumnValue(&sEval, 0, ret);
        cnt++;
    }
    mdbEvalFinalize(&sEval);
    if( rc==JSI_BREAK ) {
        rc = JSI_OK;
    }
    Jsi_DSFree(&dStr);
    return rc;
}
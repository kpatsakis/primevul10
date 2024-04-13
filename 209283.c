static Jsi_RC SqliteTransactionCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int rc;
    Jsi_Db *jdb;

    int argc = Jsi_ValueGetLength(interp, args);
    if (!(jdb = dbGetDbHandle(interp, _this, funcPtr))) return JSI_ERROR;

    Jsi_Value *pScript;
    const char *zBegin = "SAVEPOINT _jsi_transaction";

    if( jdb->nTransaction==0 && argc==2 ) {
        Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 0);
        static const char *TTYPE_strs[] = {
            "deferred",   "exclusive",  "immediate", 0
        };
        enum TTYPE_enum {
            TTYPE_DEFERRED, TTYPE_EXCLUSIVE, TTYPE_IMMEDIATE
        };
        int ttype;
        if( Jsi_ValueGetIndex(interp, arg, TTYPE_strs, "transaction type",
                              0, &ttype) ) {
            return JSI_ERROR;
        }
        switch( (enum TTYPE_enum)ttype ) {
        case TTYPE_DEFERRED:    /* no-op */
            ;
            break;
        case TTYPE_EXCLUSIVE:
            zBegin = "BEGIN EXCLUSIVE";
            break;
        case TTYPE_IMMEDIATE:
            zBegin = "BEGIN IMMEDIATE";
            break;
        }
    }
    pScript = Jsi_ValueArrayIndex(interp, args, argc-1);
    if(!Jsi_ValueIsFunction(interp, pScript)) 
        return Jsi_LogError("expected function");

    /* Run the SQLite BEGIN command to open a transaction or savepoint. */
    jdb->disableAuth++;
    rc = sqlite3_exec(jdb->db, zBegin, 0, 0 ,0);
    jdb->disableAuth--;
    if( rc!=SQLITE_OK ) 
        return Jsi_LogError("%s", sqlite3_errmsg(jdb->db));
    jdb->nTransaction++;

    /* Evaluate the function , then
    ** call function dbTransPostCmd() to commit (or rollback) the transaction
    ** or savepoint.  */
    Jsi_RC rv = Jsi_FunctionInvoke(interp, pScript, NULL, NULL, NULL);
    rv = dbTransPostCmd(jdb, interp, rv);
    return rv;
}
static Jsi_RC dbTransPostCmd(
    Jsi_Db *jdb,                       /* Sqlite3Db for $db */
    Jsi_Interp *interp,                  /* Jsi interpreter */
    Jsi_RC result                           /* Result of evaluating SCRIPT */
) {
    static const char *azEnd[] = {
        "RELEASE _jsi_transaction",        /* rc==JSI_ERROR, nTransaction!=0 */
        "COMMIT",                          /* rc!=JSI_ERROR, nTransaction==0 */
        "ROLLBACK TO _jsi_transaction ; RELEASE _jsi_transaction",
        "ROLLBACK"                         /* rc==JSI_ERROR, nTransaction==0 */
    };
    Jsi_RC rc = result;
    const char *zEnd;

    jdb->nTransaction--;
    zEnd = azEnd[(rc==JSI_ERROR)*2 + (jdb->nTransaction==0)];

    jdb->disableAuth++;
    if( sqlite3_exec(jdb->db, zEnd, 0, 0, 0)) {
        /* This is a tricky scenario to handle. The most likely cause of an
        ** error is that the exec() above was an attempt to commit the
        ** top-level transaction that returned SQLITE_BUSY. Or, less likely,
        ** that an IO-error has occured. In either case, throw a Jsi exception
        ** and try to rollback the transaction.
        **
        ** But it could also be that the user executed one or more BEGIN,
        ** COMMIT, SAVEPOINT, RELEASE or ROLLBACK commands that are confusing
        ** this method's logic. Not clear how this would be best handled.
        */
        if( rc!=JSI_ERROR ) {
            Jsi_LogError("%s", sqlite3_errmsg(jdb->db));
            rc = JSI_ERROR;
        }
        sqlite3_exec(jdb->db, "ROLLBACK", 0, 0, 0);
    }
    jdb->disableAuth--;

    return rc;
}
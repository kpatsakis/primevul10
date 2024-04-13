void dbSetupCallbacks(Jsi_Db *jdb, Jsi_Db *ojdb)
{
    if (jdb->onAuth && (!ojdb || !ojdb->onAuth) )
        sqlite3_set_authorizer(jdb->db, dbAuthCallback, jdb);
    else
        sqlite3_set_authorizer(jdb->db, 0, 0);

    if (jdb->onCommit && (!ojdb || !ojdb->onCommit) )
        sqlite3_commit_hook(jdb->db, dbCommitHandler, jdb);
    else
        sqlite3_commit_hook(jdb->db, 0, 0);

    if (jdb->onBusy && (!ojdb || !ojdb->onBusy) )
        sqlite3_busy_handler(jdb->db, dbBusyHandler, jdb);
    else
        sqlite3_busy_handler(jdb->db, 0, 0);
    
    if (jdb->onTrace && (!ojdb || !ojdb->onTrace) )
        sqlite3_trace(jdb->db, dbTraceHandler, jdb);
    else
        sqlite3_trace(jdb->db, 0, 0);

    if (jdb->onNeedCollate && (!ojdb || !ojdb->onNeedCollate) )
        sqlite3_collation_needed(jdb->db, jdb, dbCollateNeeded);
    else
        sqlite3_collation_needed(jdb->db, 0, 0);

    if (jdb->onUpdate && (!ojdb || !ojdb->onUpdate) )
        sqlite3_update_hook(jdb->db, dbUpdateHandler, jdb);
    else
        sqlite3_update_hook(jdb->db, 0, 0);

    if (jdb->onWalHook && (!ojdb || !ojdb->onWalHook) )
        sqlite3_wal_hook(jdb->db, dbWalHandler, jdb);
    else
        sqlite3_wal_hook(jdb->db, 0, 0);

    if (jdb->onRollback && (!ojdb || !ojdb->onRollback) )
        sqlite3_rollback_hook(jdb->db, dbRollbackHandler, jdb);
    else
        sqlite3_rollback_hook(jdb->db, 0, 0);

    if (jdb->onProfile && (!ojdb || !ojdb->onProfile) )
        sqlite3_profile(jdb->db, dbProfileHandler, jdb);
    else
        sqlite3_profile(jdb->db, 0, 0);

    if (jdb->onProgress && jdb->progressSteps && (!ojdb || !ojdb->onProgress || ojdb->progressSteps != jdb->progressSteps) )
        sqlite3_progress_handler(jdb->db, jdb->progressSteps, dbProgressHandler, jdb);
    else
        sqlite3_progress_handler(jdb->db, 0, 0, 0);
    
    if (!ojdb || jdb->load != ojdb->load)
        sqlite3_enable_load_extension(jdb->db, jdb->load);

    if (!ojdb || jdb->timeout != ojdb->timeout)
        sqlite3_busy_timeout( jdb->db, jdb->timeout );

/*    if (jdb->onUnlock && (!ojdb || !ojdb->onUnlock) )
        sqlite3_unlock_notify(jdb->db, dbUnlockNotify, (void*)jdb);
    else
        sqlite3_unlock_notify(jdb->db, 0, 0);
        */
}
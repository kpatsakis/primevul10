int sqlite3_rekey_v2(sqlite3 *db, const char *zDb, const void *pKey, int nKey) {
  CODEC_TRACE("sqlite3_rekey_v2: entered db=%p zDb=%s pKey=%s, nKey=%d\n", db, zDb, (char *)pKey, nKey);
  if(db && pKey && nKey) {
    int db_index = sqlcipher_find_db_index(db, zDb);
    struct Db *pDb = &db->aDb[db_index];
    CODEC_TRACE("sqlite3_rekey_v2: database pDb=%p db_index:%d\n", pDb, db_index);
    if(pDb->pBt) {
      codec_ctx *ctx;
      int rc, page_count;
      Pgno pgno;
      PgHdr *page;
      Pager *pPager = pDb->pBt->pBt->pPager;

      ctx = (codec_ctx*) sqlite3PagerGetCodec(pDb->pBt->pBt->pPager);
     
      if(ctx == NULL) { 
        /* there was no codec attached to this database, so this should do nothing! */ 
        CODEC_TRACE("sqlite3_rekey_v2: no codec attached to db, exiting\n");
        return SQLITE_OK;
      }

      CODEC_TRACE_MUTEX("sqlite3_rekey_v2: entering database mutex %p\n", db->mutex);
      sqlite3_mutex_enter(db->mutex);
      CODEC_TRACE_MUTEX("sqlite3_rekey_v2: entered database mutex %p\n", db->mutex);

      codec_set_pass_key(db, db_index, pKey, nKey, CIPHER_WRITE_CTX);
    
      /* do stuff here to rewrite the database 
      ** 1. Create a transaction on the database
      ** 2. Iterate through each page, reading it and then writing it.
      ** 3. If that goes ok then commit and put ctx->rekey into ctx->key
      **    note: don't deallocate rekey since it may be used in a subsequent iteration 
      */
      rc = sqlite3BtreeBeginTrans(pDb->pBt, 1, 0); /* begin write transaction */
      sqlite3PagerPagecount(pPager, &page_count);
      for(pgno = 1; rc == SQLITE_OK && pgno <= (unsigned int)page_count; pgno++) { /* pgno's start at 1 see pager.c:pagerAcquire */
        if(!sqlite3pager_is_mj_pgno(pPager, pgno)) { /* skip this page (see pager.c:pagerAcquire for reasoning) */
          rc = sqlite3PagerGet(pPager, pgno, &page, 0);
          if(rc == SQLITE_OK) { /* write page see pager_incr_changecounter for example */
            rc = sqlite3PagerWrite(page);
            if(rc == SQLITE_OK) {
              sqlite3PagerUnref(page);
            } else {
             CODEC_TRACE("sqlite3_rekey_v2: error %d occurred writing page %d\n", rc, pgno);  
            }
          } else {
             CODEC_TRACE("sqlite3_rekey_v2: error %d occurred getting page %d\n", rc, pgno);  
          }
        } 
      }

      /* if commit was successful commit and copy the rekey data to current key, else rollback to release locks */
      if(rc == SQLITE_OK) { 
        CODEC_TRACE("sqlite3_rekey_v2: committing\n");
        rc = sqlite3BtreeCommit(pDb->pBt); 
        sqlcipher_codec_key_copy(ctx, CIPHER_WRITE_CTX);
      } else {
        CODEC_TRACE("sqlite3_rekey_v2: rollback\n");
        sqlite3BtreeRollback(pDb->pBt, SQLITE_ABORT_ROLLBACK, 0);
      }

      CODEC_TRACE_MUTEX("sqlite3_rekey_v2: leaving database mutex %p\n", db->mutex);
      sqlite3_mutex_leave(db->mutex);
      CODEC_TRACE_MUTEX("sqlite3_rekey_v2: left database mutex %p\n", db->mutex);
    }
    return SQLITE_OK;
  }
  return SQLITE_ERROR;
}